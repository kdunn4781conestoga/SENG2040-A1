/*
* FILE			: FileReceive.cpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-02-07
* DESCRIPTION	: This file holds all of the class definitions for FileReceive.h
*/

#include "FileReceive.h"

FileReceive::FileReceive() : FileTransfer(NULL)
{
	this->state = Listening;
}

void FileReceive::Setup()
{
}

char* FileReceive::GetPacket()
{
	return nullptr;
}

int FileReceive::ProcessPacket(const char* packet)
{
	return 0;
}
