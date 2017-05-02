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
	char coapVersion;
	char msgType;
	char tokenLength;
//	char code; // kod wiadomoœci CoAP - definiuje czy wiadomoœæ jest ¿¹daniem, czy informacj¹ o b³êdzie
	char codeClass;
	char codeDetails;
	char messageID[2];
	char * token = nullptr;
	char * options = nullptr;
	char * payload = nullptr;

	bool parseHeader(char * header);

public:
	// Funkcja parsuj¹ca wiadomoœæ w postaci binarnej do postaci obiektu klasy CoapMessage
	bool parse(char * msg);
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

};

#endif

