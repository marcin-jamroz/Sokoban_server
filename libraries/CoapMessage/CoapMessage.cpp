/*
 Name:		CoapMessage.cpp
 Created:	07.04.2017 17:09:39
 Author:	jaroslaw
 Editor:	http://www.visualmicro.com
*/


#include "CoapMessage.h"
#include <BitOperations.h>
#include "CoapUtils.h"


bool CoapMessage::parse(unsigned char msg[], int length)
{
	unsigned int position = 0; // numer obs�ugiwanego bajtu wiadomo�ci
	// unsigned int length = sizeof(msg); // d�ugo�� obs�ugiwanej wiadomo�ci CoAPa; sizeof(unsigned char) = 1
	unsigned char header[4] = { 0 }; // tablica nag��wka wiadomo�ci
	memcpy(header, msg, 4); // kopiowanie zawarto�ci 4 pierwszych bajt�w z wiadomo�ci do tablicy nag��wka
	position += 4;
	if (parseHeader(header)) {
		if (tokenLength != 0 && position < length) {
			token = new unsigned char[tokenLength];
			memcpy(token, &msg[position], tokenLength); // kopiowanie z wiadomo�ci zawarto�ci tokena
			position += tokenLength;
		}
		while ( position < length) {
			
			
			
			if (msg[position] == 0b11111111) { // sprawdzenie czy nie wyst�puje znacznik payloada
				position++;
				payloadLength = length - position;
				payload = new unsigned char[payloadLength];
				memcpy(payload, &msg[position], payloadLength);
				position += length - position;
			}
			else {
				parseOptions(msg, position, length);
			}
		}
		return true;
	}
	return false;
}

//unsigned char* CoapMessage::toPacket() {
//	unsigned char* header = createHeader(token, msgType, codeClass, codeDetails, msgId[]);
//}

unsigned char* CoapMessage::createHeader(unsigned char token, unsigned char msgType, unsigned char codeClass, unsigned char codeDetails, unsigned char msgId[]) {
	/*
	Format nag��wka wiadomo�ci CoAP:
	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|Ver| T |  TKL  |      Code     |          Message ID           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/

	unsigned char* header = new unsigned char[4]; //ca�y  nag��wek CoAPa ma 4 bajty (32 bity)
	unsigned char * bytePointer = &header[0]; //wska�nik na pierwszy bajt

	// Funkcja setBits numeruje bity od prawej tzn 1110 to 0 jest bitem numer 0 
	BitOperations::setBits(bytePointer, coapVersion, 6); // ustawia wersje protokolu na 6 bicie od prawej
	BitOperations::setBits(bytePointer, msgType, 4);//ustawienie 2 bit�w od czwartego jako msgType
	BitOperations::setBits(bytePointer, tokenLength, 0);//ustawienie d�ugo�ci tokena TKL

	bytePointer = &header[1];//przej�cie na kolejny bajt
	BitOperations::setBits(bytePointer, codeClass, 5);
	BitOperations::setBits(bytePointer, codeDetails, 0);

	header[2] = msgId[0];
	header[3] = msgId[1];

	return header;
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

void CoapMessage::getUriPath(String &dest)
{
	dest = uriPath;
}

unsigned char* CoapMessage::getToken()
{
	return token;
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

bool CoapMessage::parseOptions(unsigned char * message, unsigned int &position, int length) {
	unsigned int previousOptionNumber = 0; // poprzednia delta dla pierwszego to zero
	
	//dop�ki nie sko�cz� si� opcje (nie zacznie payload) wczytuj opcje
	while (position < length  && message[position] != 0b11111111)
	{
		
	
	unsigned int optionDelta = message[position] >> 4;// delta obecnej opcji
	//	Serial.print("optionDelta=");
	//	Serial.println(message[position],HEX);

	unsigned int optionLength = message[position++] & 0b00001111;	//d�ugo�� warto�ci opcji 

		
		
		unsigned int optionNumber = optionDelta + previousOptionNumber;
		
		previousOptionNumber = optionNumber;
		
	//	Serial.print("OptionDelta=");
	//	Serial.println(optionDelta);
		Serial.print("OptionNumber=");
		Serial.println(optionNumber);
		Serial.print("OptionLength=");
		Serial.println(optionLength);
	
		switch (optionNumber)
		{
		case CoapUtils::OptionNumber::URI_PATH:
			//memcpy(&uriPath, &message[position], optionLength);
			for(int i =0; i<optionLength;i++){
			uriPath = String(uriPath + (char)message[position+i]);
			}
			break;
		case CoapUtils::OptionNumber::ACCEPT:
			memcpy(&accept, &message[position], optionLength);
			break;
		case CoapUtils::OptionNumber::CONTENT_FORMAT:
			memcpy(&contentFormat, &message[position], optionLength);
			break;
		}
		position += optionLength;
	}
	Serial.println("Wyszedlem z while z parseOptions");
}


CoapMessage::~CoapMessage() {
	Serial.println("Destruktor on");
	if (token) delete token;
	if (payload) delete payload;
}
