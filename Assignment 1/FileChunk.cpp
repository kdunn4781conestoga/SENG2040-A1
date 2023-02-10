/*
* FILE			: FileChunk.cpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-02-07
* DESCRIPTION	: This file holds all of the class definitions for FileChunk.h
*/

#include "FileChunk.h"

FileChunk::FileChunk(int index)
{
	this->index = index;
	this->header = "";
	this->data = std::vector<char>();

	this->succeeded = false;
}

FileChunk::~FileChunk()
{
	
}

void FileChunk::ReadPacket(const char* packet)
{
}

bool FileChunk::AppendData(const char data)
{
	// doesn't append data if it's going to go over chunk size
	if (this->data.size() + 1 >= CHUNK_SIZE)
	{
		return false;
	}

	this->data.push_back(data);

	return true;
}

void FileChunk::GenerateHeader()
{
	// generates the header
}

char* FileChunk::GetPacket()
{
	// combines the header and packet
	char packet[256] = { 0 };

	for (int i = 0; i < data.size(); i++)
	{
		packet[i] = data[i];
	}

	return packet;
}
