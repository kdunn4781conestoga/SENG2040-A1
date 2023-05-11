/*
* FILE			: FileClient.cpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-01-31
* DESCRIPTION	: This file holds all of the class definitions for FileClient.h
*/

#include "FileClient.h"

FileClient::FileClient(std::string filename, bool testing) : FileTransfer(filename, testing)
{
	this->state = Disconnected;
	this->currentIndex = 0;
}

/*
	Name	:   Initialize
	Purpose :   this is used to setup for file transfer
	Inputs	:	NONE
	Outputs	:	NONE
	Returns	:	NONE
*/
void FileClient::Initialize()
{
	FileTransfer::Initialize();

	this->state = Connected;

	// opens file
	if (!Open("rb"))
	{
		state = Disconnecting;
		throw; // TODO
	}

	// gets length of file
	fseek(file, 0, SEEK_END);

	totalLength = ftell(file);

	// closes file
	Close();

	SetConnected(true);
}

/*
	Name	:   GetPacket
	Purpose :   this is used to receive the packet to send
	Inputs	:	NONE
	Outputs	:	NONE
	Returns	:	char*	|	pointer to the packet
*/
char* FileClient::GetPacket()
{
	if (IsConnected())
	{
		state = Sending;

		FileChunk* chunk = NULL;

		/*
		* reads from file and generates a chunk only if it's the start of the program
		* or the current index doesn't match the last index
		*/
		if (lastIndex == -1 || currentIndex != lastIndex)
		{
			if (file == NULL && !Open("rb"))
			{
				return NULL;
			}

			// goes to specific position in file
			fseek(file, currentLength, SEEK_SET);

			chunk = new FileChunk();

			// appends file contents to chunk
			char buffer[2] = { 0 };
			while (fread(buffer, 1, 1, file) != 0) {
				if (!chunk->AppendData(buffer[0]))
				{
					break;
				}
			}

			currentLength += chunk->GetData().size(); // keeps track of where in the file it stopped at

			if (!Close())
			{
				return NULL;
			}

			// adds to sentChunk to keep track of it
			sentChunks.push_back(*chunk);
			lastIndex = currentIndex;
		}
		else
		{
			// sends the previous chunk if there's no response from the server
			chunk = &sentChunks.back();
		}

		// checks for end of file
		if (currentLength >= totalLength)
		{
			// generates the hash and adds it to the header
			std::string hash = GenerateFileHash();

			chunk->CreateHeader(filename, totalLength, currentIndex, hash.c_str());
		}
		else
		{
			chunk->CreateHeader(filename, totalLength, currentIndex);
		}

		return chunk->GetPacket();
	}

	return NULL;
}

/*
	Name	:   ProcessPacket
	Purpose :   this is used to process 
	Inputs	:	packet	|	const char*	|	the packet
	Outputs	:	NONE
	Returns	:	int
*/
int FileClient::ParsePacket(const char* packet)
{
	if (IsConnected())
	{
		state = Receiving;

		// converts received packet to chunk for later processing
		if (packet != NULL && strcmp(packet, "") != 0)
		{
			FileChunk chunk;

			chunk.ReadPacket(packet);

			if (chunk.HasSucceeded())
			{
				receivedChunks.push_back(chunk);
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
int FileClient::ProcessPacket()
{
	state = Validating;

	// loops through sent chunks (usually only 1 but just in case there are more)
	for (int s = 0; s < sentChunks.size(); s++)
	{
		bool done = false;

		FileChunk* sChunk = &sentChunks[s];

		// loops through received chunks
		for (int r = 0; r < receivedChunks.size(); r++)
		{
			FileChunk* rChunk = &receivedChunks[r];

			// skips this chunk if the indexes don't match
			if (rChunk->GetIndex() != currentIndex)
			{
				continue;
			}

			// checks if the header length matches the data size
			if (sChunk->GetHeader().length == rChunk->GetData().size())
			{
				// proceeds
				done = true;

				// check if end of file
				if (currentLength >= totalLength)
				{
					// if chunk received has no checksum then it wasn't sent successfully
					if (rChunk->GetHeader().checksum != NULL)
						valid = true;

					state = Disconnecting;

					// ends program
					SetFinished(true);
				}

				break;
			}
		}

		// resets variables
		if (done)
		{
			lastIndex = currentIndex;
			currentIndex++;

			sentChunks.clear();
			receivedChunks.clear();

			break;
		}
	}

	return 0;
}