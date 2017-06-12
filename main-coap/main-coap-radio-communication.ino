
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
  unsigned char payload[2];
  int packetLength = 0;
  
  //dla statusu potencjometru i lampki, to samo - > odesłanie value w payload
  sprintf(payload, "%u", value); //kopiowanie  value do payloadu
  setResponseMessageFields(responseMessage, coapMessage, messageID, payload); //SPRAWIDZC CZY DOBRZE TE REFERENCJE SĄ PRZEKAZYWANE I OBIEKTY/TABLICE DO FUNKCJI :)
  unsigned char * packet = responseMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc
  sendUdpResponse(coapMessage, packet, packetLength);
  delete packet;
}

void handleRadioRequest(short option, short value)    //przeciążenie do obsługi zasobów o dużym czasie dostępu
{

}

void sendUdpResponse(CoapMessage coapMessage, unsigned char * packet, int packetLength) {
  Udp.beginPacket(coapMessage.getRemoteIPAddress(), coapMessage.getRemotePort());
  Udp.write(packet, packetLength);
  Udp.endPacket();
}

void setResponseMessageFields(CoapMessage responseMessage,  CoapMessage coapMessage, uint8_t messageID[], unsigned char payload[2]) {
  responseMessage.setHeader(coapMessage.getToken(), coapMessage.getTokenLength(), CoapUtils::MessageType::NON, CoapUtils::ResponseCode::SUCCESS, CoapUtils::SuccessResponseCode::CONTENT, messageID);
  responseMessage.setContentFormat(0);
  responseMessage.setPayload(payload, sizeof(payload));
}


