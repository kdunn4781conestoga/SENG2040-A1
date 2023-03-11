/*
* FILE			: FileClient.cpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-01-31
* DESCRIPTION	: This file holds all of the class definitions for FileClient.h
*/

#include "FileClient.h"

FileClient::FileClient(const char* filename) : FileTransfer(filename)
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
	this->state = Connected;
	this->currentIndex = 0;
	this->lastIndex = -1;
	this->receivedChunks.clear();
	this->sentChunks.clear();

	// opens file
	fopen_s(&file, filename.c_str(), "rb");

	if (file == NULL)
	{
		state = Disconnecting;
		throw; // TODO
	}

	// gets length of file
	fseek(file, 0, SEEK_END);

	totalLength = ftell(file);

	// closes file
	if (fclose(file))
	{
		state = Disconnecting;
		throw; // TODO
	}

	file = NULL;

	SetConnected(true);
}

char* FileClient::GetPacket()
{
	if (IsConnected())
	{
		state = Sending;

		FileChunk* chunk = NULL;

		if (lastIndex == -1 || currentIndex != lastIndex)
		{
			if (file == NULL && !Open("rb"))
			{
				return NULL;
			}

			fseek(file, currentLength, SEEK_SET);

			chunk = new FileChunk();

			char buffer[2] = { 0 };
			while (fread(buffer, 1, 1, file) != 0) {
				if (!chunk->AppendData(buffer[0]) || chunk->GetData().size() > CHUNK_SIZE)
				{
					break;
				}
			}

			currentLength += chunk->GetData().size(); // keeps track of where in the file it stopped at

			if (!Close())
			{
				return NULL;
			}

			sentChunks.push_back(*chunk);
			lastIndex = currentIndex;
		}
		else
		{
			chunk = &sentChunks.back();
		}

		if (currentLength >= totalLength)
		{
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

	for (int s = 0; s < sentChunks.size(); s++)
	{
		bool done = false;

		FileChunk* sChunk = &sentChunks[s];

		for (int r = 0; r < receivedChunks.size(); r++)
		{
			FileChunk* rChunk = &receivedChunks[r];

			if (rChunk->GetIndex() != currentIndex)
			{
				continue;
			}

			if (sChunk->GetHeader().length == rChunk->GetData().size())
			{
				done = true;

				if (currentLength >= totalLength)
				{
					if (rChunk->GetHeader().checksum != NULL)
						valid = true;

					state = Disconnecting;

					SetFinished(true);
				}

				break;
			}
		}

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