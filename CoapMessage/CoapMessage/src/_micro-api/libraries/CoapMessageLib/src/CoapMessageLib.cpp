/*
 Name:		CoapMessageLib.cpp
 Created:	07.04.2017 17:09:39
 Author:	jaroslaw
 Editor:	http://www.visualmicro.com
*/

#include "CoapMessageLib.h"


bool CoapMessageLib::parseHeader(char * header)
{
	/* header_1stByte = header[0];
	code = header[1]; */

	coapVersion = header[0] >> 5;
	if (coapVersion != 0b01)
		return false;
	msgType = (header[0] & 0b00110000) >> 4;
	tokenLength = header[0] & 0b1111;
	
	codeClass = header[1] >> 4;
	codeDetails = header[1] & 0b1111;

	messageID[0] = header[2];
	messageID[1] = header[3];

	return true;
}

bool CoapMessageLib::parse(char * msg)
{
	char * header = strncpy(header, msg, 4);
	if (parseHeader(header)) {

		return true;
	}
	return false;
}

char * CoapMessageLib::toPacket()
{
	return nullptr;
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
