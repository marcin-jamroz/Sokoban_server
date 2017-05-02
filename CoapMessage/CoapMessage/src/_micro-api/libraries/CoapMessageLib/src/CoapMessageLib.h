/*
 Name:		CoapMessageLib.h
 Created:	07.04.2017 17:09:39
 Author:	jaroslaw
 Editor:	http://www.visualmicro.com
*/

#ifndef _CoapMessageLib_h
#define _CoapMessageLib_h

#include <Arduino.h>
#include <Coap.h>
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
	char coapVersion;
	char msgType;
	char tokenLength;
//	char code; // kod wiadomo�ci CoAP - definiuje czy wiadomo�� jest ��daniem, czy informacj� o b��dzie
	char codeClass;
	char codeDetails;
	char messageID[2];
	char * token = nullptr;
	char * options = nullptr;
	char * payload = nullptr;

	bool parseHeader(char * header);

public:
	// Funkcja parsuj�ca wiadomo�� w postaci binarnej do postaci obiektu klasy CoapMessage
	bool parse(char * msg);
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

};

#endif

