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

/*
	Name	:   ReadPacket
	Purpose :   this is used to read the full packet
	Inputs	:	packet	|	const char*		|	the packet
	Outputs	:	NONE
	Returns	:	True | False
*/
bool FileChunk::ReadPacket(const char* packet)
{
	succeeded = false;

	if (packet == NULL || strcmp(packet, "") == 0) return false;

	int dataLength = -1;
	if (!ParseHeader(&dataLength, packet))
	{
		return false;
	}

	std::cout << "Header: "<< std::endl << packet << std::endl << std::endl;

	char* cPos = (char*)packet + header.size();
	while (data.size() >= dataLength)
	{
		AppendData(*cPos);

		cPos++;
	}

	succeeded = true;

	return true;
}

/*
	Name	:   AppendData
	Purpose :   this is used to append the data
	Inputs	:	data	|	const char*		|	the data being sent
	Outputs	:	NONE
	Returns	:	True | False
*/
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


/*
	Name	:   GenerateHeader
	Purpose :   this is used to generate the header
	Inputs	:	filename	|	 string		|	gets the filename to add to the header
	Outputs	:	NONE
	Returns	:	NONE
*/
void FileChunk::GenerateHeader(std::string filename)
{
	// generates the header
	std::stringstream ss;
	ss << "FILENAME: " << filename << std::endl;
	ss << "INDEX: " << index << std::endl;
	ss << "LENGTH: " << GetLength() << std::endl;

	header = ss.str();
}


/*
	Name	:   ParseHeader
	Purpose :   this is used to parse the header  
	Inputs	:	datalength	|	int*			|	pointer to the length of the data
				packet		|	const char*		|	the packet
	Outputs	:	NONE
	Returns	:	True | False
*/
bool FileChunk::ParseHeader(int *dataLength, const char* packet)
{
	std::stringstream ss(packet);

	std::string filename;

	int headerLines = 0;
	std::string line;
	while (true)
	{
		std::getline(ss, line);

		if (line == "\n" || line == "\r")
		{
			break;
		}

		if (filename.empty() && line.rfind("FILENAME: ", 0) == 0)
		{
			filename = line.substr(10);
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
	}

	return headerLines >= 3;
}


/*
	Name	:   GetPacket
	Purpose :   this is used to get the packet info
	Inputs	:	NONE
	Outputs	:	NONE
	Returns	:	packet	|	returns the pointer to the packet
*/
char* FileChunk::GetPacket()
{
	if (header.empty())
	{
		return NULL;
	}

	// combines the header and packet
	char packet[CHUNK_SIZE + HEADER_SIZE] = {0};

	strcat_s(packet, CHUNK_SIZE + HEADER_SIZE, header.c_str());
	strcat_s(packet, CHUNK_SIZE + HEADER_SIZE, "\n");

	for (int i = 0; i < data.size(); i++)
	{
		packet[i + header.length()] = data[i];
	}

	return packet;
}
