/*
 Name:		CoapMessage.cpp
 Created:	07.04.2017 17:09:39
 Author:	jaroslaw
 Editor:	http://www.visualmicro.com
*/


#include "CoapMessage.h"
#include <BitOperations.h>
#include "CoapUtils.h"
#include <DebugUtils.h>


bool CoapMessage::parse(unsigned char msg[], int length, IPAddress remoteIPAddress, int remotePort)
{

	this->remoteIPAddress = remoteIPAddress;
	this->remotePort = remotePort;

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
		while ( position < length) {
			
			
			
			if (msg[position] == 0b11111111) { // sprawdzenie czy nie wystêpuje znacznik payloada
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

unsigned char * CoapMessage::toPacket(int &packetLength) {
	unsigned char* header = createHeader();
	/*
	//Serial.println("toPacket poczatek");
	for (int i = 0; i < 4; i++) {
		//Serial.print(header[i], HEX);
		//Serial.print(", ");
		if (!(i % 10) && i!=0) //Serial.println();
	}
	*/
	//   Content-Format tylko    tymczasowo !!!
	uint8_t optionDelta = 12;
	uint8_t optionLength = 1;
	uint8_t optionDescriptionLength = 1;

	unsigned char option = (optionDelta << 4) | optionLength;
	unsigned char optionValue = this->contentFormat;
	int optionBytes = 1 + 1;
	
	packetLength = 4 + this->tokenLength + optionDescriptionLength + optionLength;

	if (this->payloadLength > 0)
		packetLength += 1 + this->payloadLength;

	unsigned char * packet = new unsigned char[packetLength];
	memcpy(packet, header, 4);
	delete header;
	memcpy(&packet[4], this->token, this->tokenLength);
	memcpy(&packet[4 + tokenLength], &option, optionDescriptionLength);
	memcpy(&packet[4 + tokenLength + 1], &optionValue, optionLength);
	if (this->payloadLength > 0)
	{
		packet[4 + tokenLength + 1 + 1] = 255; // payload marker
		memcpy(&packet[4 + tokenLength + 1 + 1 + 1], this->payload, this->payloadLength);
	}

	//Serial.println("toPacket koniec");
	return packet;
}

unsigned char* CoapMessage::createHeader() {
	/*
	Format nag³ówka wiadomoœci CoAP:
	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|Ver| T |  TKL  |      Code     |          Message ID           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/

	unsigned char* header = new unsigned char[4]; //ca³y  nag³ówek CoAPa ma 4 bajty (32 bity)
	unsigned char * bytePointer = &header[0]; //wskaŸnik na pierwszy bajt
	/*
	// Funkcja setBits numeruje bity od prawej tzn 1110 to 0 jest bitem numer 0 
	BitOperations::setBits(bytePointer, this->coapVersion, 6); // ustawia wersje protokolu na 6 bicie od prawej
	BitOperations::setBits(bytePointer, this->msgType, 4);//ustawienie 2 bitów od czwartego jako msgType
	BitOperations::setBits(bytePointer, this->tokenLength, 0);//ustawienie d³ugoœci tokena TKL

	bytePointer = &header[1];//przejœcie na kolejny bajt
	BitOperations::setBits(bytePointer, this->codeClass, 5);
	BitOperations::setBits(bytePointer, this->codeDetails, 0);

	header[2] = this->messageID[0];
	header[3] = this->messageID[1];
	*/

	// Nowa metoda bez BitOperations - DZIA£A - sprawdzone
	header[0] = (COAP_VERSION << 6) | (msgType << 4) | (tokenLength);
	header[1] = (codeClass << 5) | codeDetails;
	memcpy(&header[2], &messageID, 2);

	return header;
}

// ============== Metody aet...



void CoapMessage::setHeader(unsigned char * token, uint8_t tokenLength, uint8_t msgType, uint8_t codeClass, uint8_t codeDetails, uint16_t messageID)
{
	this->tokenLength = tokenLength;
	this->token = new unsigned char[tokenLength];
	memcpy(this->token, token, tokenLength);


	this->msgType = msgType;
	this->codeClass = codeClass;
	this->codeDetails = codeDetails;

	this->messageID = messageID;

}

void CoapMessage::setContentFormat(uint8_t contentFormat)
{
	this->contentFormat = contentFormat;
}

void CoapMessage::setPayload(unsigned char * payload, uint8_t payloadLength)
{
	////Serial.print("Wchodzi do setPayload");
	this->payloadLength = payloadLength;
	////Serial.print("Payload length=");
	////Serial.println(payloadLength);
	this->payload = new unsigned char[payloadLength];
	memcpy(this->payload, payload, payloadLength);
}

// ============== Metody get...


uint8_t CoapMessage::getCoapVersion()
{
	return COAP_VERSION;
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

uint8_t CoapMessage::getObserve()
{
	return observe;
}

unsigned char* CoapMessage::getToken()
{
	return token;
}

unsigned char* CoapMessage::getPayload()
{
	return payload;
}

uint8_t CoapMessage::getPayloadLength(){
	return payloadLength;
}

uint16_t CoapMessage::getMessageID()
{
	return messageID;
}

IPAddress CoapMessage::getRemoteIPAddress() {
	return remoteIPAddress;
}

void CoapMessage::setRemoteIPAddress(IPAddress ipAddress){
	remoteIPAddress = ipAddress;
}
int CoapMessage::getRemotePort() {
	return remotePort;
}

void CoapMessage::setRemotePort(int port){
	remotePort = port;
}


// =============================funckje prywatne  =============================
bool CoapMessage::parseHeader(unsigned char * header)
{
	/* header_1stByte = header[0];
	code = header[1]; */

	int coapVersion = header[0] >> 6;
	if (coapVersion != COAP_VERSION)
		return false;
	msgType = (header[0] & 0b00110000) >> 4;
	tokenLength = header[0] & 0b1111;

	codeClass = header[1] >> 5;
	codeDetails = header[1] & 0b1111;

	memcpy(&messageID, &header[2], 2);

	return true;
}

bool CoapMessage::parseOptions(unsigned char * message, unsigned int &position, int length) {
	unsigned int previousOptionNumber = 0; // poprzednia delta dla pierwszego to zero
	
	//dopóki nie skoñcz¹ siê opcje (nie zacznie payload) wczytuj opcje
	while (position < length  && message[position] != 0b11111111)
	{
		
	
	unsigned int optionDelta = message[position] >> 4;// delta obecnej opcji
	//	//Serial.print("optionDelta=");
	//	//Serial.println(message[position],HEX);

	unsigned int optionLength = message[position++] & 0b00001111;	//d³ugoœæ wartoœci opcji 

		
		
		unsigned int optionNumber = optionDelta + previousOptionNumber;
		
		previousOptionNumber = optionNumber;
		
	//	//Serial.print("OptionDelta=");
	//	//Serial.println(optionDelta);
	//	//Serial.print("OptionNumber=");
	//	//Serial.println(optionNumber);
	//	//Serial.print("OptionLength=");
	//	//Serial.println(optionLength);
	
		switch (optionNumber)
		{
		case CoapUtils::OptionNumber::URI_PATH:
			//memcpy(&uriPath, &message[position], optionLength);
			if (uriPath.length() > 0) 
				uriPath = String(uriPath + '/');

			for(int i = 0; i < optionLength; i++) {
				char uriPathChar = (char)message[position + i];
				
				uriPath = String(uriPath + uriPathChar);
				//Serial.println(uriPath);
			}
			//Serial.println(uriPath);
			break;
			
		case CoapUtils::OptionNumber::ACCEPT:
			memcpy(&accept, &message[position], optionLength);
			break;
			
		case CoapUtils::OptionNumber::CONTENT_FORMAT:
			memcpy(&contentFormat, &message[position], optionLength);
			break;
			
		case CoapUtils::OptionNumber::OBSERVE:
		
		if(optionLength == 0)
		{
			observe = 0;
		}
		else{
			memcpy(&observe, &message[position], optionLength);
			////Serial.print("Wartosc observe=");
			////Serial.println(observe);
			////Serial.print("Dlugosc opcji observe=");
			////Serial.println(optionLength);
		}
			break;
		}
		position += optionLength;
	}
	////Serial.println("Wyszedlem z while z parseOptions");
}


CoapMessage::~CoapMessage() {
	////Serial.println("Destruktor CoapMessage");
	if (token) delete token;
	if (payload) delete payload;
}
