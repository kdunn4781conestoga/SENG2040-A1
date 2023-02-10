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

#define CHUNK_SIZE 128

class FileChunk
{
public:
	FileChunk(int index);
	~FileChunk();

	void ReadPacket(const char* packet);
	bool AppendData(const char data);
	char* GetPacket();

	inline int GetLength() { return data.size(); }
protected:
	void GenerateHeader();
private:
	int index;
	std::string header;
	std::vector<char> data;

	bool succeeded;
};

