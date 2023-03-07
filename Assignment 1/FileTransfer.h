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

#define CHECKSUM_LENGTH 32

/*
* CLASS    : FileTransfer
* PURPOSE  : This is an abstract class for handling file transfer
*/
class FileTransfer
{
public:
	FileTransfer(const std::string filename);
	~FileTransfer();

	virtual void Initialize() = 0;
	virtual char* GetPacket() = 0;
	virtual int ParsePacket(const char* packet) = 0;
	virtual int ProcessPacket() = 0;
	
	inline virtual void SetConnected(bool connected) { this->connected = connected; }
	inline bool IsConnected() { return this->connected; }

	inline bool IsFinished() { return finished; }
protected:
	bool Open(const char* mode);
	bool Close();

	inline void SetFinished(bool finished) { this->finished = finished; }

	std::string filename;

	FILE* file;

	long currentLength;
	long totalLength;

	int lastIndex;
	int currentIndex;

	std::vector<FileChunk> sentChunks;
	std::vector<FileChunk> receivedChunks;
private:
	bool connected;
	bool finished;
};

