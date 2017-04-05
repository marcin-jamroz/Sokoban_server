#pragma once
enum RequestMethod {
	EMPTY = 0,
	GET = 1,
	POST = 2,
	PUT = 3,
	DELETE = 4
};
enum ResponseCode {
	REQUEST = 0,
	SUCCESS = 2,
	CLIENT_ERROR = 4,
	SERVER_ERROR = 5
};
enum MessageType {
	CON = 0, NON = 1, ACK = 2, RST = 3
};

// Funkcja ustawiająca nagłówek wiadomości CoAP
void setHeader(char * header, int versionNo, MessageType msgType, int tokenLength, ResponseCode code, int codeDetails, char msgId[]);