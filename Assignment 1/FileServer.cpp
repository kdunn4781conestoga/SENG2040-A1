/*
* FILE			: FileServer.cpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-02-07
* DESCRIPTION	: This file holds all of the class definitions for FileServer.h
*/

#include "FileServer.h"

FileServer::FileServer() : FileTransfer("")
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
	this->state = Waiting;
	this->currentIndex = 0;
	this->lastIndex = -1;
	this->receivedChunks.clear();
	this->sentChunks.clear();

	SetConnected(true);
}

/*
	Name	:   GetPacket
	Purpose :   this is used to receive the packet
	Inputs	:	packet	|	const char*		|	the packet
	Outputs	:	NONE
	Returns	:	nullptr	|	pointer to the packet
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

			chunk->CreateHeader(filename, totalLength, chunk->GetIndex(), chunk->GetHeader().checksum);

			if (chunk->GetHeader().checksum != NULL)
			{
				state = Listening;

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

	while (!receivedChunks.empty())
	{
		FileChunk *chunk = &receivedChunks.back();

		if (chunk != NULL && chunk->GetIndex() == currentIndex && chunk->GetData().size() == chunk->GetHeader().length)
		{
			if (chunk->GetIndex() == 0) {
				this->filename = chunk->GetHeader().filename + ".tmp";
				this->totalLength = chunk->GetHeader().filesize;

				// resets file to 0 bytes
				if (!Open("wb")) {
					return -1;
				}

				Close();
			}
			else if (this->filename.empty()) {
				state = Sending;
				return -1;
			}

			if (!Open("ab"))
			{
				return -1;
			}

			fseek(file, currentLength, SEEK_SET);

			std::vector<char> data = chunk->GetData();

			for (int i = 0; i < data.size(); i++)
			{
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
				std::cout << "Verifying file..." << std::endl;
			}

			lastIndex = currentIndex;
			currentIndex++;

			sentChunks.clear();

			sentChunks.push_back(*chunk);

			receivedChunks.clear();

			break;			
		}

		receivedChunks.pop_back();
	}

	return 0;
}