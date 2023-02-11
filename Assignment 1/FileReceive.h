/*
* FILE			: FileReceive.h
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-02-07
*/

#pragma once

#include "FileTransfer.h"

/*
* CLASS    : FileReceive
* PURPOSE  : This class handles receiving files between connections
*/
class FileReceive : public FileTransfer
{
public:
	enum State
	{
		Listening,
		Connecting,
		Waiting,
		Receiving,
		Verifying,
		Sending
	};

	FileReceive();

	virtual void Setup();
	virtual void SetConnected();
	virtual int GetPacket(char* packet, const int packetSize);
	virtual int ProcessPacket(const char* packet);

	inline State GetState() { return state; }
private:
	State state;
};

