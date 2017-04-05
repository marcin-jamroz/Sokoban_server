// coapConsoleApp.cpp: Określa punkt wejścia dla aplikacji konsoli.
//

#include "stdafx.h"
#include "bitOperations.h"
#include "coap.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 



int main()
{
	char header[4] = { 0 };
	char version = 0b01;
	char msgType = CoAP::MessageType::ACK;
	char len = 3;
	char code = CoAP::ResponseCode::SERVER_ERROR;
	int details = 1;
	CoAP::setHeader(header, version, msgType, len, code, details, "01");
	for (int i = 0; i < 4; i++) {
		printf("header[%d]: ", i);
		printf(BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(header[i]));
	}
	char a = 0b00000001;
	printf("%d\n", BitOperations::getBit(a, 3));
	BitOperations::setBits(&a, 0b1, 2);
	printf("%d", a);
	char znak;
	scanf("%c", &znak);
	return 0;
}

