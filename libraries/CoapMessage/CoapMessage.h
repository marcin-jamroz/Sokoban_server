
#ifndef CoapMessage_h
#define CoapMessage_h

#include <Arduino.h>
#include <stdint.h>
#include <string.h>
// Klasa reprezentuj�ca wiadomo�� protoko�u CoAP
class CoapMessage {
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
	uint8_t codeClass;  //0, 2, 4 ,5 
	uint8_t codeDetails; // 3 bity, numer, patrz, klasa ni�ej
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
	//parsuje wiadomosci otrzymuj�c wska�nik wiadomo�ci oraz pozycj� pocz�tku opcji
	bool parseOptions(unsigned char * message, unsigned int &position);

public:

	// Funkcja parsuj�ca wiadomo�� w postaci binarnej do postaci obiektu klasy CoapMessage
	bool parse(unsigned char msg[], int length);

	//funkcja ustawia header wiadomosci coap
	void setHeader(unsigned char token, unsigned char msgType, unsigned char codeClass, int codeDetails, char msgId[]);


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

	~CoapMessage();
};

#endif