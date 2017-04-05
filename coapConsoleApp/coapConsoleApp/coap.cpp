#include "coap.h"
#include "bitOperations.h"

// Funkcja ustawiaj¹ca nag³ówek wiadomoœci CoAP
void setHeader(char * header, int versionNo, MessageType msgType, int tokenLength, ResponseCode code, int codeDetails, char msgId[]) {
	/*
	Format nag³ówka wiadomoœci CoAP:
	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|Ver| T |  TKL  |      Code     |          Message ID           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	char headerTmp[4] = { 0 }; // nag³ówek CoAPa ma 4 bajty (32 bity)
	char * byte = &header[0];
	// Funkcja setBits numeruje bity od prawej tzn 1110 to 0 jest bitem numer 0 
	setBits(byte, versionNo, 6); // ustawia wersje protokolu na 6 bicie od prawej
	setBits(byte, msgType, 4);
	setBits(byte, tokenLength, 0);
	/// printf("byte1: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(*byte));

	byte = &header[1];
	setBits(byte, code, 5);
	setBits(byte, codeDetails, 0);
	/// printf("byte2: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(*byte));

	header[2] = msgId[0];
	header[3] = msgId[1];
	/// printf("msgID: " BYTE_TO_BINARY_PATTERN" " BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(msgId[0]), BYTE_TO_BINARY(msgId[1]));
}

void setToken(char *coapMessage, char token[]) {

}