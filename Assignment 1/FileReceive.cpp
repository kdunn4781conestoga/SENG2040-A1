/*
* FILE			: FileReceive.cpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-02-07
* DESCRIPTION	: This file holds all of the class definitions for FileReceive.h
*/

#include "FileReceive.h"

FileReceive::FileReceive() : FileTransfer("")
{
	this->state = Listening;
}

/*
	Name	:   Setup
	Purpose :   this is used to setup the file transfer for receiving
	Inputs	:	NONE
	Outputs	:	NONE
	Returns	:	NONE
*/
void FileReceive::Setup()
{
	state = Receiving;
}

/*
	Name	:   GetPacket
	Purpose :   this is used to receive the packet
	Inputs	:	packet	|	const char*		|	the packet
	Outputs	:	NONE
	Returns	:	nullptr	|	pointer to the packet
*/
char* FileReceive::GetPacket()
{
	if (state == Sending)
	{
		// send packet information
		if (lastChunk != NULL)
		{
			return lastChunk->GetPacket();
		}

		state = Receiving;
	}

	return nullptr;
}

/*
	Name	:   ProcessPacket
	Purpose :   this is used to process the packet
	Inputs	:	packet	|	const char*		|	the packet
	Outputs	:	NONE
	Returns	:	int
*/
int FileReceive::ProcessPacket(const char* packet)
{
	if (state == Receiving)
	{
		if (packet == NULL || strcmp(packet, "") == 0)
		{
			lastChunk = new FileChunk(-1);
		}
		else
		{
			FileChunk* chunk = new FileChunk(-1);
			chunk->ReadPacket(packet);

			if (chunk->HasSucceeded())
			{
				Open("wb");

				fseek(file, currentLength, SEEK_SET);

				std::vector<char> data = chunk->GetData();

				for (int i = 0; i < data.size(); i++)
				{
					fwrite(&data[i], 1, 1, file);
				}

				Close();

				currentLength += CHUNK_SIZE;

				lastChunk = chunk;
			}
			else
			{
				lastChunk = new FileChunk(-1);
			}
		}

		state = Sending;

		return 0;
	}

	return -1;
}
