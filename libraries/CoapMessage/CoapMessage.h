#ifndef CoapMessage_h
#define CoapMessage_h

#include <Arduino.h>
#include <stdint.h>
#include <string.h>
#include <Ethernet.h>
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
	uint16_t messageID;

	unsigned char * token = nullptr;
	
	unsigned char * payload = nullptr;
	uint8_t payloadLength;


	//uri_path - np. "lampka"
	//accept - format preferowamy dla odpowiedzi 
	//content Format - format payloadu (text/plain, application/json itp)
	String uriPath;
	uint8_t accept, contentFormat, observe;

	IPAddress remoteIPAddress;
	int remotePort;

	bool parseHeader(unsigned char * header);
	//parsuje wiadomosci otrzymuj¹c wskaŸnik wiadomoœci oraz pozycjê pocz¹tku opcji
	bool parseOptions(unsigned char * message, unsigned int &position, int length);

	unsigned char* createHeader();


public:

	// Funkcja parsuj¹ca wiadomoœæ w postaci binarnej do postaci obiektu klasy CoapMessage
	bool parse(unsigned char msg[], int length, IPAddress remoteIPAddress, int remotePort);

	
	// Funkcja tworz¹ca pakiet z wiadomoœci CoAP. Trzeba pamiêtaæ o zwolnieniu pamiêci gdy skoñczy siê z nim pracê;
	unsigned char* toPacket(int &packetLength);
	
	// ================= Metody set... ================== //

	// Ustawia naglowek wiadomosci
	void setHeader(unsigned char * token, uint8_t tokenLength, uint8_t msgType, uint8_t codeClass, uint8_t codeDetails, uint16_t messageID);

	// ---- OPCJE
	// Ustawia contentFormat
	void setContentFormat(uint8_t contentFormat);

	// Ustawia payload wiadomosci
	void setPayload(unsigned char * payload, uint8_t payloadLength);

	

	// ================= Metody get... ================== //

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

	// Zwraca dlugosc payloadu
	uint8_t getPayloadLength();

	// Zwraca messageID
	uint16_t getMessageID();

	// Zwraca UriPath
	void getUriPath(String &dest);
	
	// zwraca wartosc opcji observe
	uint8_t getObserve();

	// Zwraca payload	
	unsigned char* getPayload();
	
	// Zwraca token
	unsigned char* getToken();

	// Zwraca adres IP z którego przysz³a wiadomoœæ CoAPa 
	IPAddress getRemoteIPAddress();

	// Zwraca port z którego przysz³a wiadomoœæ CoAPa
	int getRemotePort();

	// destruktor obiektu
	~CoapMessage();
};

#endif