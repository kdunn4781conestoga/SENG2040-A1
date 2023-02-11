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

bool FileChunk::ReadPacket(std::string *filename, const char* packet)
{
	succeeded = false;

	if (packet == NULL || strcmp(packet, "") == 0) return false;

	int dataLength = -1;
	if (!ParseHeader(filename, &dataLength, packet))
	{
		return false;
	}

	char* cPos = (char*)packet + header.size();
	while (data.size() < dataLength)
	{
		AppendData(*cPos);

		cPos++;
	}

	succeeded = true;

	return true;
}

bool FileChunk::AppendData(const char data)
{
	// doesn't append data if it's going to go over chunk size
	if (this->data.size() + 1 > CHUNK_SIZE)
	{
		return false;
	}

	this->data.push_back(data);
	dataLength++;

	return true;
}

void FileChunk::GenerateHeader(std::string filename)
{
	// generates the header
	std::stringstream ss;
	ss << "FILENAME: " << filename << std::endl;
	ss << "INDEX: " << index << std::endl;
	ss << "LENGTH: " << GetLength() << std::endl;

	header = ss.str();
}

bool FileChunk::ParseHeader(std::string *filename, int *dataLength, const char* packet)
{
	std::stringstream ss(packet);

	std::string f;

	int headerLines = 0;
	int count = 0;
	std::string line;
	while (true)
	{
		std::getline(ss, line);

		if (count >= MAX_HEADER_LINES || line == "\n" || line == "\r" || line == "\r\n")
		{
			break;
		}

		if (f.empty() && line.rfind("FILENAME: ", 0) == 0)
		{
			f = line.substr(10);
			headerLines++;
		}

		if (*dataLength == -1 && line.rfind("LENGTH: ", 0) == 0)
		{
			*dataLength = std::stol(line.substr(8));
			headerLines++;
		}

		if (index == -1 && line.rfind("INDEX: ", 0) == 0)
		{
			index = std::stoi(line.substr(7));
			headerLines++;
		}

		header += line + "\n";
		count++;
	}

	if (filename != NULL)
	{
		*filename = "test-" + f;
	}

	return headerLines >= 3;
}

int FileChunk::GetPacket(char* packet, const int packetSize)
{
	if (header.empty() || data.empty())
	{
		return -1;
	}

	// combines the header and packet
	strcat_s(packet, packetSize, header.c_str());
	strcat_s(packet, packetSize, "\n");

	for (int i = 0; i < dataLength; i++)
	{
		packet[i + header.size()] = data[i];
	}

	return 0;
}

std::vector<char> FileChunk::GetData()
{
	return data;
}
