/*
* FILE			: main.cpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-01-24
* DESCRIPTION	: This program is a modification of the example to 
*			      send files over UDP using a custom protocol.
* 
* SOURCE		: Reliability and Flow Control Example
*				  From "Networking for Game Programmers" - http://www.gaffer.org/networking-for-game-programmers
*				  Author: Glenn Fiedler <gaffer@gaffer.org>
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Net.h"
#include "FileClient.h"
#include "FileServer.h"

//#define SHOW_ACKS

using namespace std;
using namespace net;

const int ServerPort = 30000;
const int ClientPort = 30001;
const int ProtocolId = 0x11223344;
const float DeltaTime = 1.0f / 90.0f; //1.0f / 30.0f;
const float SendRate = 1.0f / 90.0f; //1.0f / 30.0f;
const float TimeOut = 10.0f;
const int PacketSize = 256;

class FlowControl
{
public:
	
	FlowControl()
	{
		printf( "flow control initialized\n" );
		Reset();
	}
	
	void Reset()
	{
		mode = Bad;
		penalty_time = 4.0f;
		good_conditions_time = 0.0f;
		penalty_reduction_accumulator = 0.0f;
	}
	
	void Update( float deltaTime, float rtt )
	{
		const float RTT_Threshold = 250.0f;

		if ( mode == Good )
		{
			if ( rtt > RTT_Threshold )
			{
				printf( "*** dropping to bad mode ***\n" );
				mode = Bad;
				if ( good_conditions_time < 10.0f && penalty_time < 60.0f )
				{
					penalty_time *= 2.0f;
					if ( penalty_time > 60.0f )
						penalty_time = 60.0f;
					printf( "penalty time increased to %.1f\n", penalty_time );
				}
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				return;
			}
			
			good_conditions_time += deltaTime;
			penalty_reduction_accumulator += deltaTime;
			
			if ( penalty_reduction_accumulator > 10.0f && penalty_time > 1.0f )
			{
				penalty_time /= 2.0f;
				if ( penalty_time < 1.0f )
					penalty_time = 1.0f;
				printf( "penalty time reduced to %.1f\n", penalty_time );
				penalty_reduction_accumulator = 0.0f;
			}
		}
		
		if ( mode == Bad )
		{
			if ( rtt <= RTT_Threshold )
				good_conditions_time += deltaTime;
			else
				good_conditions_time = 0.0f;
				
			if ( good_conditions_time > penalty_time )
			{
				printf( "*** upgrading to good mode ***\n" );
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				mode = Good;
				return;
			}
		}
	}
	
	float GetSendRate()
	{
		return mode == Good ? 30.0f : 10.0f;
	}
	
private:

	enum Mode
	{
		Good,
		Bad
	};

	Mode mode;
	float penalty_time;
	float good_conditions_time;
	float penalty_reduction_accumulator;
};

// ----------------------------------------------

const int FILENAME_LENGTH = 128;

int main( int argc, char * argv[] )
{
	// parse command line

	enum Mode
	{
		Client,
		Server
	};

	Mode mode = Server;
	Address address;

	// create instance of FileTransfer class
	FileTransfer* fileTransfer = NULL;

	// variables for storing filenames

	if ( argc >= 3 )
	{
		int a,b,c,d;
		if ( sscanf_s( argv[1], "%d.%d.%d.%d", &a, &b, &c, &d ) )
		{
			mode = Client;
			address = Address(a,b,c,d,ServerPort);
		}

		char filename[FILENAME_LENGTH];
		if (sscanf_s(argv[2], "%s", &filename, sizeof(filename)))
		{
			fileTransfer = new FileClient((char*)filename);
		}
	}

	if (mode == Server)
		fileTransfer = new FileServer();

	// initialize

	if ( !InitializeSockets() )
	{
		printf( "failed to initialize sockets\n" );
		return 1;
	}

	ReliableConnection connection( ProtocolId, TimeOut );
	
	const int port = mode == Server ? ServerPort : ClientPort;
	
	if ( !connection.Start( port ) )
	{
		printf( "could not start connection on port %d\n", port );
		return 1;
	}
	
	if ( mode == Client )
		connection.Connect( address );
	else
		connection.Listen();
	
	bool connected = false;
	float sendAccumulator = 0.0f;
	float statsAccumulator = 0.0f;
	
	FlowControl flowControl;

	// constructor would require the input/output filenames and the mode
	// if the mode is Client then it will fload() the file storing it in the instance
	
	while ( true )
	{
		// update flow control
		
		if ( connection.IsConnected() )
			flowControl.Update( DeltaTime, connection.GetReliabilitySystem().GetRoundTripTime() * 1000.0f );
		
		const float sendRate = flowControl.GetSendRate();

		// detect changes in connection state

		if ( mode == Server && connected && !connection.IsConnected() )
		{
			flowControl.Reset();
			printf( "reset flow control\n" );
			connected = false;
		}

		if ( !connected && connection.IsConnected() )
		{
			printf( "client connected to server\n" );
			connected = true;
			fileTransfer->Initialize();
		}
		
		if ( !connected && connection.ConnectFailed() )
		{
			printf( "connection failed\n" );
			break;
		}

		if (fileTransfer->IsFinished())
		{
			printf("file transfer completed\n");
			connection.Stop();
			break;
		}

		fileTransfer->SetConnected(connected);
		
		// send and receive packets
		
		sendAccumulator += DeltaTime;
		
		// sending packets
		while ( sendAccumulator > 1.0f / sendRate )
		{
			char packet[PacketSize];
			memset( packet, 0, sizeof( packet ) );

			char* filePacket = fileTransfer->GetPacket();

			if (filePacket != NULL)
			{
				strncpy_s(packet, PacketSize, filePacket, _TRUNCATE);
			}

			// CLIENT
			// check if any packets need to be resent
			// call function in the FileTransfer instance for getting packet
			// the function will include the protocol header and part of file
			// FileTransfer will keep track of the current and total ammount sent
			// and also in what order they were sent in.
			// sends a specific packet indicating that it has transferred everything
			// with the checksum
			// 
			// SERVER
			// checks to see if any packets were received from FileTransfer
			// calls FileTransfer function to get a packet to send back to the client
			// that confirms the packet was sent
			connection.SendPacket((unsigned char*)packet, sizeof( packet ) );
			sendAccumulator -= 1.0f / sendRate;
		}
		
		// receiving packets
		while ( true )
		{
			char packet[256];
			int bytes_read = connection.ReceivePacket((unsigned char*)packet, sizeof(packet) );
			if ( bytes_read == 0 )
				break;
			else
			{
				fileTransfer->ParsePacket(packet);
			}
			// 
			// CLIENT
			// here it checks for the server's return message
			// FileTransfer processes it and determines if it's valid or needs to be resent
			// this information would be stored in the instance of FileTransfer
			// 
			// SERVER
			// validates the packet and determines if the protocol is valid
			// sends the packet to a FileTransfer function that saves it locally
			// FileTransfer will store the total amount received
			// 
			// if the packet received is to indicate end of file transfer
			// generate a checksum for the file and compare it with the one received
		}
		
		// show packets that were acked this frame
		
		#ifdef SHOW_ACKS
		unsigned int * acks = NULL;
		int ack_count = 0;
		connection.GetReliabilitySystem().GetAcks( &acks, ack_count );
		if ( ack_count > 0 )
		{
			printf( "acks: %d", acks[0] );
			for ( int i = 1; i < ack_count; ++i )
				printf( ",%d", acks[i] );
			printf( "\n" );
		}
		#endif

		// update connection
		
		connection.Update( DeltaTime );

		// show connection stats
		
		statsAccumulator += DeltaTime;

		while ( statsAccumulator >= 0.25f && connection.IsConnected() )
		{
			float rtt = connection.GetReliabilitySystem().GetRoundTripTime();
			
			unsigned int sent_packets = connection.GetReliabilitySystem().GetSentPackets();
			unsigned int acked_packets = connection.GetReliabilitySystem().GetAckedPackets();
			unsigned int lost_packets = connection.GetReliabilitySystem().GetLostPackets();
			
			float sent_bandwidth = connection.GetReliabilitySystem().GetSentBandwidth();
			float acked_bandwidth = connection.GetReliabilitySystem().GetAckedBandwidth();
			
			printf( "rtt %.1fms, sent %d, acked %d, lost %d (%.1f%%), sent bandwidth = %.1fkbps, acked bandwidth = %.1fkbps\n", 
				rtt * 1000.0f, sent_packets, acked_packets, lost_packets, 
				sent_packets > 0.0f ? (float) lost_packets / (float) sent_packets * 100.0f : 0.0f, 
				sent_bandwidth, acked_bandwidth );
			
			statsAccumulator -= 0.25f;
		}

		// process packets
		fileTransfer->ProcessPacket();

		net::wait( DeltaTime );
	}
	
	ShutdownSockets();

	return 0;
}
