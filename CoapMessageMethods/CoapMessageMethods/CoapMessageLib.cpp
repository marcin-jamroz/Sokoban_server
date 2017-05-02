/*
 Name:		CoapMessageLib.cpp
 Created:	07.04.2017 17:09:39
 Author:	jaroslaw
 Editor:	http://www.visualmicro.com
*/

#include "stdafx.h"
#include <string.h>

#include "CoapMessageLib.h"
#include "BitOperations.h"
bool CoapMessageLib::parseHeader(unsigned char * header)
{
	/* header_1stByte = header[0];
	code = header[1]; */

	coapVersion = header[0] >> 6;
	if (coapVersion != 0b01)
		return false;
	msgType = (header[0] & 0b00110000) >> 4;
	tokenLength = header[0] & 0b1111;
	
	codeClass = header[1] >> 5;
	codeDetails = header[1] & 0b1111;

	messageID[0] = header[2];
	messageID[1] = header[3];

	return true;
}

bool CoapMessageLib::parse(unsigned char msg[], int length)
{
	unsigned int position = 0; // numer obs³ugiwanego bajtu wiadomoœci
	// unsigned int length = sizeof(msg); // d³ugoœæ obs³ugiwanej wiadomoœci CoAPa; sizeof(unsigned char) = 1
	unsigned char header[4] = { 0 }; // tablica nag³ówka wiadomoœci
	memcpy(header, msg, 4); // kopiowanie zawartoœci 4 pierwszych bajtów z wiadomoœci do tablicy nag³ówka
	position += 4;
	if (parseHeader(header)) {
		if (tokenLength != 0 && position < length) {
			token = new unsigned char[tokenLength];
			memcpy(token, &msg[position], tokenLength); // kopiowanie z wiadomoœci zawartoœci tokena
			position += tokenLength;
		}
		while (msg[position] && position < length) {
			if (msg[position++] == 0b11111111) { // sprawdzenie czy nie wystêpuje znacznik payloada
				payload = new unsigned char[length - position + 1];
				memcpy(payload, &msg[position], length - position);
				payload[length - position] = 0;
				position += length - position;
				
			}
		}
		return true;
	}
	return false;
}

char * CoapMessageLib::toPacket()
{
	// Nag³ówek
	/*
	Format nag³ówka wiadomoœci CoAP:
	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|Ver| T |  TKL  |      Code     |          Message ID           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	unsigned char headerTmp[4] = { 0 }; // nag³ówek CoAPa ma 4 bajty (32 bity)
	unsigned char * byte = &headerTmp[0];
	// Funkcja setBits numeruje bity od prawej tzn 1110 to 0 jest bitem numer 0 
	BitOperations::setBits(byte, coapVersion, 6); // ustawia wersje protokolu na 6 bicie od prawej
	BitOperations::setBits(byte, msgType, 4);
	BitOperations::setBits(byte, tokenLength, 0);

	byte = &headerTmp[1];
	BitOperations::setBits(byte, codeClass, 5);
	BitOperations::setBits(byte, codeDetails, 0);
	/// printf("byte2: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(*byte));

	headerTmp[2] = messageID[0];
	headerTmp[3] = messageID[1];
}

uint8_t CoapMessageLib::getCoapVersion()
{
	return coapVersion;
}

uint8_t CoapMessageLib::getMessageType()
{
	return msgType;
}

uint8_t CoapMessageLib::getCodeClass()
{
	return codeClass;
}

uint8_t CoapMessageLib::getCodeDetails()
{
	return codeDetails;
}

uint8_t CoapMessageLib::getTokenLength()
{
	return tokenLength;
}
CoapMessageLib::~CoapMessageLib() {
	if (token) delete token;
	if (options) delete options;
	if (payload) delete payload;
}
