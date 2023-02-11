/*
* FILE			: FileTransfer.cpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-01-31
* DESCRIPTION	: This file holds all of the class definitions for FileTransfer.h
*/

#include "FileTransfer.h"


FileTransfer::FileTransfer(const std::string filename)
{
	this->file = NULL;
	this->currentLength = 0;
	this->totalLength = 0;
	this->filename = filename;
	this->currentIndex = -1;
	this->lastChunk = NULL;
}

FileTransfer::~FileTransfer()
{
	Close();
}

bool FileTransfer::Open(const char* mode)
{
	fopen_s(&file, filename.c_str(), mode);

	return file != NULL;
}

bool FileTransfer::Close()
{
	if (fclose(file))
	{
		return false;
	}

	file = NULL;

	return true;
}