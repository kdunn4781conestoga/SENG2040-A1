/*
* FILE			: FileValidatorcpp
* PROJECT		: Assignment 1
* PROGRAMMERS	: Kyle Dunn & David Czachor
* FIRST VERSION : 2023-02-10
* DESCRIPTION	: This file holds the checksum algorithm
*/

#include "FileValidator.h"

/*
    Name	:   Fletcher16
    Purpose :   this is a copy of John Fletchers checksum algo.
    Inputs	:	data	|	uint8_t	|	the packet
                length  |   int     |   the length of the packet
    Outputs	:	NONE
    Returns	:	the checksum value
*/
uint16_t Fletcher16(uint8_t* data, int length)
{
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;
    int index;

    for (index = 0; index < length; ++index)
    {
        sum1 = (sum1 + data[index]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (sum2 << 8) | sum1;
}