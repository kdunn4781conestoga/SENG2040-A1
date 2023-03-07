/*
* FILE			: FileChunk.cpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-02-07
* DESCRIPTION	: This file holds all of the class definitions for FileChunk.h
*/

#include "FileChunk.h"

FileChunk::FileChunk()
{
	this->header = SimpleProtocol();
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

	//std::cout << "Header: "<< std::endl << packet << std::endl << std::endl;

	if (header.checksum != NULL)
	{
		printf("End?\n");
	}

	char* cPos = (char*)packet + GenerateHeader().size() + 1;
	while (true)
	{
		AppendData(*cPos);

		if (data.size() >= dataLength)
			break;
		else
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
	if (this->data.size() + 1 > CHUNK_SIZE)
	{
		return false;
	}

	this->data.push_back(data);

	return true;
}


/*
	Name	:   CreateHeader
	Purpose :   this is used to create the header
	Inputs	:	filename	|	 string		|	gets the filename to add to the header
	Outputs	:	NONE
	Returns	:	NONE
*/
void FileChunk::CreateHeader(std::string filename, long filesize, int index, const char* checksum)
{
	header.filename = filename;
	header.filesize = filesize;
	header.index = index;
	header.length = data.size();

	if (checksum != NULL)
	{
		header.checksum = (char*)malloc(strlen(checksum) + 1);
		if (header.checksum != NULL)
		{
			strcpy_s(header.checksum, strlen(checksum) + 1, checksum);
		}
	}
}

/*
	Name	:   GenerateHeader
	Purpose :   this is used to generate the header
	Inputs	:	NONE
	Outputs	:	NONE
	Returns	:	NONE
*/
std::string FileChunk::GenerateHeader()
{
	// generates the header
	std::stringstream ss;
	if (!this->header.filename.empty())
	{
		ss << "FILENAME: " << this->header.filename << std::endl;
	}

	if (this->header.filesize != -1)
	{
		ss << "FILESIZE: " << this->header.filesize << std::endl;
	}

	if (this->header.index != -1)
	{
		ss << "INDEX: " << this->header.index << std::endl;
	}

	if (this->header.length != -1)
	{
		ss << "LENGTH: " << this->header.length << std::endl;
	}

	// sends checksum to indicate that it's the end of the file transfer
	if (this->header.checksum != NULL)
	{
		ss << "CHECKSUM: " << this->header.checksum << std::endl;
	}

	return ss.str();
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

	int headerLines = 0;
	std::string line;
	while (true)
	{
		std::getline(ss, line);

		if (line == "" || headerLines >= MAX_HEADER_LINES)
		{
			break;
		}

		if (line.rfind("FILENAME: ", 0) == 0)
		{
			header.filename = line.substr(10);
		}

		if (line.rfind("FILESIZE: ", 0) == 0)
		{
			header.filesize = std::stol(line.substr(10));
		}

		if (line.rfind("INDEX: ", 0) == 0)
		{
			header.index = std::stoi(line.substr(7));
		}

		if (*dataLength == -1 && line.rfind("LENGTH: ", 0) == 0)
		{
			*dataLength = std::stol(line.substr(8));
			header.length = *dataLength;
		}

		if (line.rfind("CHECKSUM: ", 0) == 0)
		{
			std::string checksum = line.substr(10);
			header.checksum = (char*)malloc(checksum.length() + 1);
			if (header.checksum != NULL)
			{
				strcpy_s(header.checksum, checksum.length() + 1, checksum.c_str());
			}
		}

		headerLines++;
	}

	return headerLines >= MAX_HEADER_LINES - 1;
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
	std::string header = GenerateHeader();

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
		packet[i + header.length() + 1] = data[i];
	}

	return packet;
}
