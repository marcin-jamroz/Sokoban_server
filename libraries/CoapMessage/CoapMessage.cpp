/*
 Name:		CoapMessage.cpp
 Created:	07.04.2017 17:09:39
 Author:	jaroslaw
 Editor:	http://www.visualmicro.com
*/


#include "CoapMessage.h"
#include <BitOperations.h>



bool CoapMessage::parse(unsigned char msg[], int length)
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
			else {
				parseOptions(msg, position);
			}
		}
		return true;
	}
	return false;
}




void CoapMessage::setHeader(unsigned char * header, int versionNo, unsigned char msgType, int tokenLength, unsigned char code, int codeDetails, char msgId[]) {
	/*
	Format nag³ówka wiadomoœci CoAP:
	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|Ver| T |  TKL  |      Code     |          Message ID           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	unsigned char headerTmp[4] = { 0 }; // nag³ówek CoAPa ma 4 bajty (32 bity)
	unsigned char * byte = &header[0];
	// Funkcja setBits numeruje bity od prawej tzn 1110 to 0 jest bitem numer 0 
	BitOperations::setBits(byte, versionNo, 6); // ustawia wersje protokolu na 6 bicie od prawej
	BitOperations::setBits(byte, msgType, 4);
	BitOperations::setBits(byte, tokenLength, 0);

	byte = &header[1];
	BitOperations::setBits(byte, code, 5);
	BitOperations::setBits(byte, codeDetails, 0);
	/// printf("byte2: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(*byte));

	header[2] = msgId[0];
	header[3] = msgId[1];
}




char * CoapMessage::toPacket()
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

uint8_t CoapMessage::getCoapVersion()
{
	return coapVersion;
}

uint8_t CoapMessage::getMessageType()
{
	return msgType;
}

uint8_t CoapMessage::getCodeClass()
{
	return codeClass;
}

uint8_t CoapMessage::getCodeDetails()
{
	return codeDetails;
}

uint8_t CoapMessage::getTokenLength()
{
	return tokenLength;
}




// =============================funckje prywatne  =============================
bool CoapMessage::parseHeader(unsigned char * header)
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

bool CoapMessage::parseOptions(unsigned char * message, unsigned int &position) {
	unsigned int previousOptionNumber = 0; // poprzednia delta dla pierwszego to zero
	
	//dopóki nie skoñcz¹ siê opcje (nie zacznie payload) wczytuj opcje
	while (message[position] && message[position] == 0b11111111)
	{
		
		unsigned int optionDelta = message[position] >> 4;// delta obecnej opcji
		unsigned int optionLength = message[position++] & 0b00001111;	//d³ugoœæ wartoœci opcji 
		
		unsigned int optionNumber = optionDelta + previousOptionNumber;
		
		previousOptionNumber = optionNumber;
		
	
		switch (optionNumber)
		{
		case OptionNumber::URI_HOST:
			memcpy(&uriHost, &message[position], optionLength);//kopiowanie bitów wartoœci opcji do odpowiedniej zmiennej obiektu CoapMessage 
			break;
		case OptionNumber::URI_PORT:
			memcpy(&uriPort, &message[position], optionLength);
			break;
		case OptionNumber::URI_PATH:
			memcpy(&uriPath, &message[position], optionLength);
			break;
		case OptionNumber::ACCEPT:
			memcpy(&accept, &message[position], optionLength);
			break;
		case OptionNumber::CONTENT_FORMAT:
			memcpy(&contentFormat, &message[position], optionLength);
			break;
		}
		position += optionLength;
	}
}


CoapMessage::~CoapMessage() {
	if (token) delete token;
	if (options) delete options;
	if (payload) delete payload;
}
