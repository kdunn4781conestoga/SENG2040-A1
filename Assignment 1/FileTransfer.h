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
	FileTransfer(const std::string filename, bool testing);
	~FileTransfer();

	std::string GenerateFileHash();

	virtual void Initialize();
	virtual char* GetPacket() = 0;
	virtual int ParsePacket(const char* packet) = 0;
	virtual int ProcessPacket() = 0;

	inline void SetTesting(bool testing) { this->testing = testing; }
	
	inline virtual void SetConnected(bool connected) { this->connected = connected; }
	inline bool IsConnected() { return this->connected; }

	inline bool IsFinished() { return finished; }
	inline bool IsValid() { return valid; }

	inline long GetCurrentLength() { return currentLength; }
	inline long GetTotalLength() { return totalLength; }
protected:
	bool Open(const char* mode);
	bool Close();

	inline void SetFinished(bool finished) { this->finished = finished; }

	bool testing;

	std::string filename;

	FILE* file;

	long currentLength;
	long totalLength;

	int lastIndex;
	int currentIndex;
	bool valid;

	std::vector<FileChunk> sentChunks;
	std::vector<FileChunk> receivedChunks;
private:

	bool connected;
	bool finished;
};

