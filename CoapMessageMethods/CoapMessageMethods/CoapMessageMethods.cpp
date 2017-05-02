// CoapMessageMethods.cpp: Okreœla punkt wejœcia dla aplikacji konsoli.
//

#include "stdafx.h"
#include "CoapMessageLib.h"
#include "CoAP.h"

int main()
{
	unsigned char header[7] = { NULL };
	unsigned char version = 0b01;
	unsigned char msgType = CoAP::MessageType::ACK;
	unsigned char len = 0;
	unsigned char code = CoAP::ResponseCode::SERVER_ERROR;
	int details = 1;
	header[4] = 0b11111111;
	header[5] = 'a';
	header[6] = 'b';
	CoAP::setHeader(header, version, msgType, len, code, details, "AB");



	CoapMessageLib coapMessage;
	int length = sizeof(header);
	coapMessage.parse(header, length);
    return 0;
}

