#include <CoAP.h>
#include <BitOperations.h>

void setup(){

  Serial.begin(115200);
  
  char header[4] = { 0 };
  char version = 0b01;
  char msgType = CoAP::MessageType::ACK;
  char len = 3;
  char code = CoAP::ResponseCode::SERVER_ERROR;
  int details = 1;
  CoAP::setHeader(header, version, msgType, len, code, details, 15);
  for (int i = 0; i < 4; i++) {
   Serial.print("header: ");
   Serial.println(i);
   Serial.println(header[i], BIN);
   
  }
}


void loop(){
  
}

