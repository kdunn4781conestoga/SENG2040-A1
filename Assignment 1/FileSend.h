/*
* FILE			: FileSend.h
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-01-31
*/

#pragma once

#include "FileChunk.h"
#include "FileTransfer.h"

/*
* CLASS    : FileSend
* PURPOSE  : This class handles sending files between connections
*/
class FileSend : public FileTransfer
{
public:
	enum State
	{
		Disconnected,
		Connecting,
		Connected,
		Sending,
		Receiving,
		Disconnecting
	};

	FileSend(const char* filename);

	virtual void Setup();
	virtual char* GetPacket();
	virtual int ProcessPacket(const char* packet);

	inline State GetState() { return state; }

private:
	State state;
};

