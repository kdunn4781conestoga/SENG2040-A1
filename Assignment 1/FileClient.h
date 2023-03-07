/*
* FILE			: FileClient.h
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-01-31
*/

#pragma once

#include "FileChunk.h"
#include "FileTransfer.h"

/*
* CLASS    : FileClient
* PURPOSE  : This class handles sending files between connections
*/
class FileClient : public FileTransfer
{
public:
	enum State
	{
		Disconnected,
		Connecting,
		Connected,
		Sending,
		Receiving,
		Validating,
		Disconnecting
	};

	FileClient(const char* filename);

	virtual void Initialize();
	virtual char* GetPacket();
	virtual int ParsePacket(const char* packet);
	virtual int ProcessPacket();

	inline State GetState() { return state; }

private:
	State state;
};

