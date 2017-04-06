#include "BitOperations.h"
#include "CoAP.h"

void CoAP::setHeader(char * header, int versionNo, char msgType, int tokenLength, char code, int codeDetails, uint16_t msgId) {
		/*
		Format nagłówka wiadomości CoAP:
		0                   1                   2                   3
		0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|Ver| T |  TKL  |      Code     |          Message ID           |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*/
		char headerTmp[4] = { 0 }; // nagłówek CoAPa ma 4 bajty (32 bity)
		char * byte = &header[0];
		// Funkcja setBits numeruje bity od prawej tzn 1110 to 0 jest bitem numer 0 
		BitOperations::setBits(byte, versionNo, 6); // ustawia wersje protokolu na 6 bicie od prawej
		BitOperations::setBits(byte, msgType, 4);
		BitOperations::setBits(byte, tokenLength, 0);

		byte = &header[1];
		BitOperations::setBits(byte, code, 5);
		BitOperations::setBits(byte, codeDetails, 0);
		/// printf("byte2: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(*byte));

		header[2] = msgId;
	}

	/*void CoAP::setToken() {

	}*/