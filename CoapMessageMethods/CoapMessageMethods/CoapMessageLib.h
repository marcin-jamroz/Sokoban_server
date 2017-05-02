#pragma once

#include <stdint.h>
#include "CoAP.h"
// Klasa reprezentuj�ca wiadomo�� protoko�u CoAP
class CoapMessageLib {
private:
	/* 
		header_1stByte - 1-szy bajt nag��wka, kt�ry zawiera:
			wersj� CoAPa	2 bity
			typ wiadomo�ci	2 bity
			d�ugo�� tokena	4 bity
	
	char header_1stByte; 
	*/
	uint8_t coapVersion = 01;
	uint8_t msgType;
	uint8_t tokenLength;
//	char code; // kod wiadomo�ci CoAP - definiuje czy wiadomo�� jest ��daniem, czy informacj� o b��dzie
	uint8_t codeClass;
	uint8_t codeDetails;
	uint8_t messageID[2];
	unsigned char * token = nullptr;
	unsigned char * options = nullptr;
	unsigned char * payload = nullptr;

	bool parseHeader(unsigned char * header);

public:
	// Funkcja parsuj�ca wiadomo�� w postaci binarnej do postaci obiektu klasy CoapMessage
	bool parse(unsigned char msg[], int length);
	// Funkcja tworz�ca pakiet z wiadomo�ci CoAP. Trzeba pami�ta� o zwolnieniu pami�ci gdy sko�czy si� z nim prac�;
	char* toPacket();
	// Zwraca wersj� CoAPa
	uint8_t getCoapVersion();
	// Zwraca typ wiadomo�ci
	uint8_t getMessageType();
	// Zwraca klas� kodu, okre�laj�c� czy wiadomo�� jest ��daniem/b��dem
	uint8_t getCodeClass();
	// Zwraca szczeg�owe dane dot. ��dania/b��du
	uint8_t getCodeDetails();
	// Zwraca dlugosc tokena
	uint8_t getTokenLength();
	// destruktor obiektu
	~CoapMessageLib();
};