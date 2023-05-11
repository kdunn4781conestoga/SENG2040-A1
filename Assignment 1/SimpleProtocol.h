/*
* FILE			: SimpleProtocol.h
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-01-24
*/

#pragma once

#include <string>

/*
* CLASS    : SimpleProtocol
* PURPOSE  : This class stores information about the 
*			 file being transferred for the protocol  
*/
static struct SimpleProtocol
{
	std::string filename;
	long filesize = -1;
	int index = -1;
	long length = -1;
	char* checksum;
};

