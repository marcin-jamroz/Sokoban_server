
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
//	char code; // kod wiadomoœci CoAP - definiuje czy wiadomoœæ jest ¿¹daniem, czy informacj¹ o b³êdzie
	uint8_t codeClass;
	uint8_t codeDetails;
	uint8_t messageID[2];
	unsigned char * token = nullptr;
	unsigned char * options = nullptr;
	unsigned char * payload = nullptr;

	String uriHost, uriPath, accept, contentFormat;
	uint8_t uriPort;

	bool parseHeader(unsigned char * header);
	//parsuje wiadomosci otrzymuj¹c wskaŸnik wiadomoœci oraz pozycjê pocz¹tku opcji
	bool parseOptions(unsigned char * message, unsigned int &position);

public:

	class OptionNumber {
	public:
		static const unsigned char URI_HOST = 3;
		static const unsigned char URI_PORT = 7;
		static const unsigned char URI_PATH = 11;
		static const unsigned char ACCEPT = 17;
		static const unsigned char CONTENT_FORMAT = 12;
	};

	class RequestMethod {
	public:
		static const unsigned char EMPTY = 0;
		static const unsigned char GET = 1;
		static const unsigned char POST = 2;
		static const unsigned char PUT = 3;
		static const unsigned char DELETE = 4;
	};
	class ResponseCode {
	public:
		static const unsigned char REQUEST = 0;
		static const unsigned char SUCCESS = 2;
		static const unsigned char CLIENT_ERROR = 4;
		static const unsigned char SERVER_ERROR = 5;
	};
	class MessageType {
	public:
		static const unsigned char CON = 0, NON = 1, ACK = 2, RST = 3;
	};

	// Funkcja parsuj¹ca wiadomoœæ w postaci binarnej do postaci obiektu klasy CoapMessage
	bool parse(unsigned char msg[], int length);

	//funkcja ustawia header wiadomosci coap
	void setHeader(unsigned char * header, int versionNo, unsigned char msgType, int tokenLength, unsigned char code, int codeDetails, char msgId[]);


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