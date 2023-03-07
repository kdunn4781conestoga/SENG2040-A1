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
	this->lastIndex = -1;
	this->connected = false;
	this->finished = false;
}

FileTransfer::~FileTransfer()
{
	Close();
}

/*
	Name	:   FileTransfer
	Purpose :   this opens the file for filetransfer
	Inputs	:	mode	|	const char*	|	the state the system is in
	Outputs	:	NONE
	Returns	:	the file
*/
bool FileTransfer::Open(const char* mode)
{
	fopen_s(&file, filename.c_str(), mode);

	return file != NULL;
}

/*
	Name	:   FileTransfer
	Purpose :   this closes the file for filetransfer
	Inputs	:	NONE
	Outputs	:	NONE
	Returns	:	the file
*/
bool FileTransfer::Close()
{
	if (fclose(file))
	{
		return false;
	}

	file = NULL;

	return true;
}