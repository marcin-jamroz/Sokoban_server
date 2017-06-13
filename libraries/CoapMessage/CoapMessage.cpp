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
	int optionsBytesLength = 0;
	unsigned char * optionsBytes = optionsToBytes(optionsBytesLength);
	
	packetLength = 4 + this->tokenLength + optionsBytesLength;
	Serial.print("packetLength: ");
	Serial.println(packetLength);
	//Serial.println(optionsBytes[optionsBytesLength-1],HEX);

	if (this->payloadLength > 0)
		packetLength += 1 + this->payloadLength;
	//
	
	unsigned char * packet = new unsigned char[packetLength];
	memcpy(packet, header, 4);
	delete header;
	memcpy(&packet[4], this->token, this->tokenLength);
	memcpy(&packet[4 + tokenLength], optionsBytes, optionsBytesLength);
	delete optionsBytes;
	if (this->payloadLength > 0)
	{
		packet[4 + tokenLength + optionsBytesLength] = 255; // payload marker
		memcpy(&packet[4 + tokenLength + optionsBytesLength + 1], this->payload, this->payloadLength);
	}
	
	//Serial.println("toPacket koniec");
	return packet;
}

unsigned char* CoapMessage::optionsToBytes(int &optionsBytesLength) {
	uint8_t optionLength = 0;
	uint8_t previousOptionNumber = 0, optionDelta = 0;
	uint8_t observeOptionLength = 0;
	uint8_t optionDescriptionLength = 1;
	
	if(isObserveEnabled)
	{
		optionDelta = 6 - previousOptionNumber;		
		// (observe > (1 << i*8) - liczenie ilosci bajtow ile moze zajac ta liczba
		for(int i = 2; (observeOptionValue[i] > 0 && i>=0); i--)
			observeOptionLength++;
		optionsBytesLength += observeOptionLength + 1; // + 1 za description	
		previousOptionNumber = 6;
	}
	// Content-Format option
	uint8_t optionContentFormatLength;
	uint8_t optionContentFormat;
	optionDelta = 12 - previousOptionNumber;
	if(contentFormat == 0)
		optionContentFormatLength = 0;
	else {
		optionContentFormatLength = 1;
	}
	optionsBytesLength += 1 + optionContentFormatLength; // option Content-Format Description + optionContentFormatLength
	unsigned char optionContentFormatDescription = (optionDelta << 4) | optionContentFormatLength;
	// Block2 option
	
	int block2OptionSize = 0;
	if(isBlock2Enabled) {
		if(block2SequenceNumber < (1 << 3)) {
			
			block2OptionSize = 1;
		}
		else if(block2SequenceNumber >= (1 << 3) && block2SequenceNumber < (1 << 11)) {
			block2OptionSize = 2;
		}
		else
		{
			block2OptionSize = 3;
		}
		optionsBytesLength += block2OptionSize;
		optionsBytesLength += 1; // block2 option description
	}
	
	unsigned char * options = new unsigned char[optionsBytesLength];
	
	int optByte = 0;
	if(isObserveEnabled) {
		Serial.println("observe enable");
		options[optByte++] = (6 << 4) | observeOptionLength;
		for(int i = observeOptionLength-1; i>=0; i--) {
			options[optByte++] = observeOptionValue[2-i];
			//Serial.println(options[optByte-1]);
		}
	}
	options[optByte++] = optionContentFormatDescription;
	if(optionContentFormatLength == 1)
		options[optByte++] = contentFormat;
	if(isBlock2Enabled) {
		options[optByte++] = ((23 - 12) << 4) | block2OptionSize; // block2 - contentFormat
		options[optByte++] = (block2SequenceNumber << (block2OptionSize*8 - 4)) | ((isMoreBlocks ? 1 : 0) << 3) | szx;	
	}
	
	return options;
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
	return observeOptionValue[2];
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


void CoapMessage::setObserveValue(bool enableFlag, uint8_t* observeValue){
	isObserveEnabled = enableFlag;
	observeOptionValue[0] = observeValue[0];
		observeOptionValue[1] = observeValue[1];
	observeOptionValue[2] = observeValue[2];


}
void CoapMessage::setBlock2Option(uint8_t sizeExponent, bool isMoreBlocks, int sequenceNumber) {
	this->isBlock2Enabled = true;
	this->szx = sizeExponent;
	this->isMoreBlocks = isMoreBlocks;
	this->block2SequenceNumber = sequenceNumber;
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
			if(optionLength == 0)
				contentFormat = 0;
			else memcpy(&contentFormat, &message[position], optionLength);
			break;
			
		case CoapUtils::OptionNumber::OBSERVE:
		
		
		if(optionLength == 0)
		{
			observeOptionValue[0] = observeOptionValue[1] = observeOptionValue[2] = 0;
		}
		else
		{
			for(int i = 0; i < optionLength; i++) 
				memcpy(&observeOptionValue[2-i], &message[position], optionLength);
		}
			break;
		}
		position += optionLength;
	}
	////Serial.println("Wyszedlem z while z parseOptions");
}

CoapMessage::CoapMessage() {
	contentFormat = 0;
	isObserveEnabled = false;
	isAcceptEnabled = false;
	isBlock2Enabled = false;
}

CoapMessage::~CoapMessage() {
	Serial.println("Destruktor CoapMessage");
	if (token) delete token;
	if (payload) delete payload;
}
