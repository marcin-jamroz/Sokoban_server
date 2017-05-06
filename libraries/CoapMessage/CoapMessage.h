
#ifndef CoapMessage_h
#define CoapMessage_h

#include <Arduino.h>
#include <stdint.h>
#include <string.h>
// Klasa reprezentuj¹ca wiadomoœæ protoko³u CoAP
class CoapMessage {
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
	uint8_t codeClass;  //0, 2, 4 ,5 
	uint8_t codeDetails; // 3 bity, numer, patrz, klasa ni¿ej
	uint8_t messageID[2];

	unsigned char * token = nullptr;
	unsigned char * payload = nullptr;

	uint8_t payloadLength;
	uint8_t tokenLength;


	//uri_path - np. "lampka"
	//accept - format preferowamy dla odpowiedzi 
	//content Format - format payloadu (text/plain, application/json itp)
	String uriPath;
	uint8_t accept, contentFormat;

	bool parseHeader(unsigned char * header);
	//parsuje wiadomosci otrzymuj¹c wskaŸnik wiadomoœci oraz pozycjê pocz¹tku opcji
	bool parseOptions(unsigned char * message, unsigned int &position);

public:

	// Funkcja parsuj¹ca wiadomoœæ w postaci binarnej do postaci obiektu klasy CoapMessage
	bool parse(unsigned char msg[], int length);

	//funkcja ustawia header wiadomosci coap
	void setHeader(unsigned char token, unsigned char msgType, unsigned char codeClass, int codeDetails, char msgId[]);


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

	~CoapMessage();
};

#endif