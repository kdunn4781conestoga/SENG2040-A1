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

#define CHUNK_SIZE 64
#define HEADER_SIZE 64
#define MAX_HEADER_LINES 5

class FileChunk
{
public:
	FileChunk(int index);
	~FileChunk();

	bool ReadPacket(std::string *filename, const char* packet);
	bool AppendData(const char data);
	int GetPacket(char* packet, const int packetSize);
	std::vector<char> GetData();
	void GenerateHeader(std::string filename);

	inline int GetLength() { return data.size() + header.length(); }
	inline int GetIndex() { return index; }
	inline bool HasSucceeded() { return succeeded; }
private:
	bool ParseHeader(std::string* filename, int* dataLength, const char* packet);

	int index;
	std::string header;
	std::vector<char> data;
	int dataLength;

	bool succeeded;
};

