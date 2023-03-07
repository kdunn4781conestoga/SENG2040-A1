/*
* FILE			: FileServer.h
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-02-07
*/

#pragma once

#include "FileTransfer.h"

/*
* CLASS    : FileServer
* PURPOSE  : This class handles receiving files between connections
*/
class FileServer : public FileTransfer
{
public:
	enum State
	{
		Listening,
		Connecting,
		Waiting,
		Receiving,
		Verifying,
		Sending,
		Finished
	};

	FileServer();

	virtual void Initialize();
	virtual char* GetPacket();
	virtual int ParsePacket(const char* packet);
	virtual int ProcessPacket();

	inline State GetState() { return state; }
private:
	State state;
};

