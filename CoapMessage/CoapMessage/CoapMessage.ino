/*
 Name:		CoapMessage.ino
 Created:	07.04.2017 17:09:39
 Author:	jaroslaw
 Editor:	http://www.visualmicro.com
*/

#include <CoAP.h>
#include "CoapMessageLib.h"

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	char header[4] = { 0 };
	char version = 0b01;
	char msgType = CoAP::MessageType::ACK;
	char len = 3;
	char code = CoAP::ResponseCode::SERVER_ERROR;
	int details = 1;
	CoAP::setHeader(header, version, msgType, len, code, details, 15);

	CoapMessageLib coapMessage;
	coapMessage.parse(header);
	Serial.print("version");
	Serial.println(coapMessage.getCoapVersion());
	Serial.print("msgType: ");
	Serial.println(coapMessage.getMessageType());
	Serial.print("tokenLength: ");
	Serial.println(coapMessage.getTokenLength());
	Serial.print("code class: ");
	Serial.println(coapMessage.getCodeClass());
	Serial.print("code details");
	Serial.println(coapMessage.getCodeDetails());
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
