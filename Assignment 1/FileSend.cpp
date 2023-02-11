/*
* FILE			: FileSend.cpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-01-31
* DESCRIPTION	: This file holds all of the class definitions for FileSend.h
*/

#include "FileSend.h"

FileSend::FileSend(const char* filename) : FileTransfer(filename)
{
	this->state = Disconnected;
	this->currentIndex = 0;
}

void FileSend::Setup()
{
	this->state = Sending;
	this->currentIndex = 0;

	// opens file
	fopen_s(&file, filename.c_str(), "rb");

	if (file == NULL)
	{
		throw; // TODO
	}

	// gets length of file
	fseek(file, 0, SEEK_END);

	totalLength = ftell(file);

	// closes file
	if (fclose(file))
	{
		throw; // TODO
	}

	file = NULL;
}

void FileSend::SetConnected()
{
	state = Sending;
}

char* FileSend::GetPacket()
{
	if (state == Sending)
	{
		FileChunk* chunk = NULL;

		if (lastChunk == NULL || currentIndex != lastChunk->GetIndex())
		{
			if (file == NULL && !Open("rb"))
			{
				return NULL;
			}

			fseek(file, currentLength, SEEK_SET);

			chunk = new FileChunk(currentIndex);

			char buffer[2] = { 0 };
			while (fread(buffer, 1, 1, file) != 0) {
				chunk->AppendData(buffer[0]);

				if (chunk->GetData().size() > CHUNK_SIZE)
				{
					break;
				}
			}

			currentLength += chunk->GetData().size(); // keeps track of where in the file it stopped at

			if (!Close())
			{
				return NULL;
			}

			lastChunk = chunk;
		}
		else
		{
			chunk = lastChunk;
		}

		if (currentLength >= totalLength)
		{
			state = Disconnecting;
		}
		else
		{
			state = Receiving;
		}

		chunk->GenerateHeader(filename);

		return chunk->GetPacket();
	}

	return 0;
}

int FileSend::ProcessPacket(const char* packet)
{
	if (state == Receiving)
	{
		FileChunk* chunk = NULL;
		if (packet == NULL || strcmp(packet, "") == 0)
		{
			lastChunk = new FileChunk(-1);
		}
		else
		{
			chunk = new FileChunk(-1);

			chunk->ReadPacket(packet);

			if (chunk->HasSucceeded())
			{
				currentIndex++;
			}
		}

		lastChunk = chunk;

		state = Sending;

		return 0;
	}

	return 0;
}
