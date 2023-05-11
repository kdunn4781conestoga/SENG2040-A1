/*
* FILE			: FileServer.cpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-02-07
* DESCRIPTION	: This file holds all of the class definitions for FileServer.h
*/

#include "FileServer.h"

FileServer::FileServer(std::string filename, bool testing) : FileTransfer(filename, testing)
{
	this->state = Listening;
}

/*
	Name	:   Initialize
	Purpose :   this is used to setup the file transfer for receiving
	Inputs	:	NONE
	Outputs	:	NONE
	Returns	:	NONE
*/
void FileServer::Initialize()
{
	FileTransfer::Initialize();

	this->state = Waiting;

	if (testing)
		srand(time(NULL));

	valid = false;
	SetFinished(false);
	SetConnected(true);
}

/*
	Name	:   GetPacket
	Purpose :   this is used to receive the packet to send
	Inputs	:	NONE
	Outputs	:	NONE
	Returns	:	char*	|	pointer to the packet
*/
char* FileServer::GetPacket()
{
	if (IsConnected())
	{
		state = Sending;

		// send packet information
		if (lastIndex != -1)
		{
			FileChunk* chunk = &sentChunks.back();

			// creates header for chunk
			chunk->CreateHeader(filename, totalLength, chunk->GetIndex(), chunk->GetHeader().checksum);

			// checks if it's received all file contents
			if (currentLength >= totalLength)
			{
				// this is null if the checksum didn't match
				if (chunk->GetHeader().checksum != NULL)
					valid = true;

				state = Listening;

				// closes program
				SetFinished(true);
			}

			return chunk->GetPacket();
		}
	}

	return nullptr;
}

/*
	Name	:   ParsePacket
	Purpose :   this is used to parse the packet
	Inputs	:	packet	|	const char*		|	the packet
	Outputs	:	NONE
	Returns	:	int
*/
int FileServer::ParsePacket(const char* packet)
{
	if (IsConnected())
	{
		state = Receiving;

		// saves packet as chunk for later processing
		if (packet != NULL && strcmp(packet, "") != 0)
		{
			FileChunk* chunk = new FileChunk();
			chunk->ReadPacket(packet);

			if (chunk->HasSucceeded())
			{
				receivedChunks.push_back(*chunk);
			}
		}

		return 0;
	}

	return -1;
}

/*
	Name	:   ProcessPacket
	Purpose :   this is used to process the packet
	Inputs	:	NONE
	Outputs	:	NONE
	Returns	:	NONE
*/
int FileServer::ProcessPacket()
{
	state = Verifying;

	// loops through all received chunks
	while (!receivedChunks.empty())
	{
		FileChunk *chunk = &receivedChunks.back();

		// some checks to make sure it's the right chunk
		if (chunk != NULL && chunk->GetIndex() == currentIndex && chunk->GetData().size() == chunk->GetHeader().length)
		{
			// run some operations for the first chunk
			if (chunk->GetIndex() == 0) {
				// adds a suffix if filename is missing
				if (this->filename.empty())
					this->filename = "saved-" + chunk->GetHeader().filename;

				// sets totalLength if this is the first chunk
				this->totalLength = chunk->GetHeader().filesize;

				// resets file to 0 bytes
				if (!Open("wb")) {
					return -1;
				}

				Close();
			}

			// append to binary file
			if (!Open("ab"))
			{
				return -1;
			}

			// got to position in file
			fseek(file, currentLength, SEEK_SET);

			// save chunk data
			std::vector<char> data = chunk->GetData();
			for (int i = 0; i < data.size(); i++)
			{
				if (testing && rand() % 2 == 0) continue; // skips writing some parts if in testing mode and the random number = 0

				fwrite(&data[i], 1, 1, file);
			}

			currentLength += data.size();

			if (!Close())
			{
				return -1;
			}

			// check if last of chunks
			if (chunk->GetHeader().checksum != NULL && currentLength >= totalLength)
			{
				// generate file hash and compare them with the checksum provided
				std::string hash = GenerateFileHash();

				if (hash != chunk->GetHeader().checksum)
				{
					// sets the checksum to NULL to indicate it failed
					chunk->CreateHeader(filename, totalLength, currentIndex);
				}
			}

			// update variables
			lastIndex = currentIndex;
			currentIndex++;

			sentChunks.clear();

			// adds chunk to sentChunks to send back to client to confirm it was successful
			sentChunks.push_back(*chunk);

			receivedChunks.clear();

			break;			
		}

		// removes the back chunk
		receivedChunks.pop_back();
	}

	return 0;
}