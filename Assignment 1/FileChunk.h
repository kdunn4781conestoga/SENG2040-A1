/*
* FILE			: FileChunk.h
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-02-07
*/

#pragma once

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <sstream>

#include "SimpleProtocol.h"

#define CHUNK_SIZE 128
#define HEADER_SIZE 100
#define MAX_HEADER_LINES 5

class FileChunk
{
public:
	FileChunk();
	~FileChunk();

	bool ReadPacket(const char* packet);
	bool AppendData(const char data);
	char* GetPacket();
	void CreateHeader(std::string filename, long filesize, int index, const char *checksum = NULL);
	std::string GenerateHeader();

	inline int GetIndex() { return header.index; }
	inline bool HasSucceeded() { return succeeded; }
	inline SimpleProtocol GetHeader(){ return (const SimpleProtocol)header; }
	inline std::vector<char> GetData() { return data; }
private:
	bool ParseHeader(int* dataLength, const char* packet);

	SimpleProtocol header;
	std::vector<char> data;

	bool succeeded;
};

