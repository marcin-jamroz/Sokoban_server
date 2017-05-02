#pragma once

#include <stdint.h>
#include "CoAP.h"
// Klasa reprezentuj¹ca wiadomoœæ protoko³u CoAP
class CoapMessageLib {
private:
	/* 
		header_1stByte - 1-szy bajt nag³ówka, który zawiera:
			wersjê CoAPa	2 bity
			typ wiadomoœci	2 bity
			d³ugoœæ tokena	4 bity
	
	char header_1stByte; 
	*/
	uint8_t coapVersion = 01;
	uint8_t msgType;
	uint8_t tokenLength;
//	char code; // kod wiadomoœci CoAP - definiuje czy wiadomoœæ jest ¿¹daniem, czy informacj¹ o b³êdzie
	uint8_t codeClass;
	uint8_t codeDetails;
	uint8_t messageID[2];
	unsigned char * token = nullptr;
	unsigned char * options = nullptr;
	unsigned char * payload = nullptr;

	bool parseHeader(unsigned char * header);

public:
	// Funkcja parsuj¹ca wiadomoœæ w postaci binarnej do postaci obiektu klasy CoapMessage
	bool parse(unsigned char msg[], int length);
	// Funkcja tworz¹ca pakiet z wiadomoœci CoAP. Trzeba pamiêtaæ o zwolnieniu pamiêci gdy skoñczy siê z nim pracê;
	char* toPacket();
	// Zwraca wersjê CoAPa
	uint8_t getCoapVersion();
	// Zwraca typ wiadomoœci
	uint8_t getMessageType();
	// Zwraca klasê kodu, okreœlaj¹c¹ czy wiadomoœæ jest ¿¹daniem/b³êdem
	uint8_t getCodeClass();
	// Zwraca szczegó³owe dane dot. ¿¹dania/b³êdu
	uint8_t getCodeDetails();
	// Zwraca dlugosc tokena
	uint8_t getTokenLength();
	// destruktor obiektu
	~CoapMessageLib();
};