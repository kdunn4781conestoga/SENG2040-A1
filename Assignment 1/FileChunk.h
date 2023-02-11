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

#define CHUNK_SIZE 128
#define HEADER_SIZE 64

class FileChunk
{
public:
	FileChunk(int index);
	~FileChunk();

	bool ReadPacket(const char* packet);
	bool AppendData(const char data);
	char* GetPacket();
	void GenerateHeader(std::string filename);

	inline int GetLength() { return data.size() + header.length(); }
	inline int GetIndex() { return index; }
	inline bool HasSucceeded() { return succeeded; }
	inline std::vector<char> GetData() { return data; }
private:
	bool ParseHeader(int* dataLength, const char* packet);

	int index;
	std::string header;
	std::vector<char> data;

	bool succeeded;
};

