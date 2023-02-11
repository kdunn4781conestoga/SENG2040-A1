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

#include "FileChunk.h"

/*
* CLASS    : FileTransfer
* PURPOSE  : This is an abstract class for handling file transfer
*/
class FileTransfer
{
public:
	FileTransfer(const std::string filename);
	~FileTransfer();

	virtual void SetConnected() = 0;
	virtual void Setup() = 0;
	virtual char* GetPacket() = 0;
	virtual int ProcessPacket(const char* packet) = 0;
protected:
	bool Open(const char* mode);
	bool Close();

	std::string filename;

	FILE* file;

	long currentLength;
	long totalLength;

	int currentIndex;

	FileChunk* lastChunk;
	FileChunk* currentChunk;

	std::list<FileChunk> chunks;
};

