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
}

void FileSend::Setup()
{
	// opens file
	fopen_s(&file, filename, "rb");

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

char* FileSend::GetPacket()
{
	if (file == NULL && !Open())
	{
		return nullptr;
	}

	fseek(file, currentLength, SEEK_SET);

	if (currentLength == 0)
		currentIndex = 0;

	FileChunk* chunk = new FileChunk(currentIndex);

	char buffer[2] = { 0 };
	while (fread(buffer, 1, 1, file) != 0) {
		chunk->AppendData(buffer[0]);

		if (chunk->GetLength() > CHUNK_SIZE)
		{
			break;
		}
	}

	currentLength += chunk->GetLength(); // keeps track of where in the file it stopped at

	if (!Close())
	{
		return nullptr;
	}

	return chunk->GetPacket();
}

int FileSend::ProcessPacket(const char* packet)
{
	return 0;
}
