#include <CoAP.h>
#include <CoapMessageLib.h>

bool debug = false;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);


}

// the loop function runs over and over again until power down or reset
void loop() {
  if (!debug) {
    debug = true;
    char header[4] = { 0 };
    uint8_t coap_version = 0b01;
    uint8_t msgType = CoAP::MessageType::ACK;
    uint8_t len = 3;
    uint8_t code = CoAP::ResponseCode::SERVER_ERROR;
    uint8_t details = 1;
    CoAP::setHeader(header, coap_version, msgType, len, code, details, 15);

    CoapMessageLib coapMessage;
    coapMessage.parse(header);
    Serial.print("coap version: ");
    Serial.println(coapMessage.getCoapVersion());
    Serial.print("msgType: ");
    Serial.println(coapMessage.getMessageType());
    Serial.print("tokenLength: ");
    Serial.println(coapMessage.getTokenLength());
    Serial.print("code class: ");
    Serial.println(coapMessage.getCodeClass());
    Serial.print("code details: ");
    Serial.println(coapMessage.getCodeDetails());
  }
}

