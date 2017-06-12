
//=======RADIO COMMUNICATION WITH REASOURCE =======


//wysyła request do Resourca wywołując w nim akcje zależną od opcji
void sendRequestViaRadio(short option) { // opcja : 0 - lampka off, 1 - lampka ON, 2 - stan lampki, 3 - stan potencjometru
  struct Request request;
  RF24NetworkHeader header(01);
  request.option = option;

  network.write(header, &request, sizeof(request));
}



//obsługa przyjścia waidomości od resourca drogą radiową, odesłanie COAP-owej wiadomości jako response
void handleRadioRequest(short option, short value, CoapMessage &coapMessage) {

  Serial.println(value);
  CoapMessage responseMessage;
  uint8_t messageID[] = {201, 201};

  int digits = 1;
  int tmpValue = value;
  while( tmpValue /= 10 )
    digits++;
  Serial.println(digits);
  unsigned char payload[digits];
  int packetLength = 0;
  
  //dla statusu potencjometru i lampki, to samo - > odesłanie value w payload
  sprintf(payload, "%u", value); //kopiowanie  value do payloadu

  //memcpy(payload, &value, sizeof(value));

  //for(int i = 0; i<sizeof(value); i++) {
  //  payload[i] -= '0';
  //}
  
  setResponseMessageFields(responseMessage, coapMessage, messageID, payload, digits); //SPRAWIDZC CZY DOBRZE TE REFERENCJE SĄ PRZEKAZYWANE I OBIEKTY/TABLICE DO FUNKCJI :)

  unsigned char * packet = responseMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc
  Serial.print("handleRadioRequest packetLen: ");
  Serial.println(packetLength);
  sendUdpResponse(coapMessage, packet, packetLength);
  delete packet;
}

void handleRadioRequest(short option, short value)    //przeciążenie do obsługi zasobów o dużym czasie dostępu
{

}

void sendUdpResponse(CoapMessage &coapMessage, unsigned char * packet, int packetLength) {
  Serial.println("sendUdpResponse");
  Udp.beginPacket(coapMessage.getRemoteIPAddress(), coapMessage.getRemotePort());
  Udp.write(packet, packetLength);
  Udp.endPacket();
}

void setResponseMessageFields(CoapMessage &responseMessage,  CoapMessage &coapMessage, uint8_t messageID[], unsigned char * payload, int payloadLength) {
  if(payload == NULL)
  {
    Serial.println("payload null");
    return;
  }
  else
  {
    Serial.println("payload nie null");

    responseMessage.setHeader(coapMessage.getToken(), coapMessage.getTokenLength(), CoapUtils::MessageType::NON, CoapUtils::ResponseCode::SUCCESS, CoapUtils::SuccessResponseCode::CONTENT, messageID);
    responseMessage.setContentFormat(0);
    responseMessage.setPayload(payload, payloadLength);
    showDebug(coapMessage);

    showDebug(responseMessage);
  }
}


