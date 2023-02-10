/*
* FILE			: FileTransfer.h
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-01-31
*/

#pragma once

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <vector>

#include "FileChunk.h"

/*
* CLASS    : FileTransfer
* PURPOSE  : This is an abstract class for handling file transfer
*/
class FileTransfer
{
public:
	FileTransfer(const char* filename);
	~FileTransfer();

	virtual void Setup() = 0;
	virtual char* GetPacket() = 0;
	virtual int ProcessPacket(const char* packet) = 0;
protected:
	bool Open();
	bool Close();

	char* filename;

	FILE* file;

	long currentLength;
	long totalLength;

	int lastIndex;
	int currentIndex;

	std::list<FileChunk> chunks;
};

