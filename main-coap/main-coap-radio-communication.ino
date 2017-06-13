
//=======RADIO COMMUNICATION WITH REASOURCE =======


//wysyła request do Resourca wywołując w nim akcje zależną od opcji
void sendRequestViaRadio(short option) { // opcja : 0 - lampka off, 1 - lampka ON, 2 - stan lampki, 3 - stan potencjometru
  struct Request request;
  RF24NetworkHeader header(01);
  request.option = option;

  network.write(header, &request, sizeof(request));
}



//obsługa przyjścia waidomości od resourca drogą radiową, odesłanie COAP-owej wiadomości jako response
void handleRadioRequest(short value, CoapMessage &coapMessage) {

  Serial.println(value);
  CoapMessage responseMessage;
  uint8_t messageID[] = {201, 201};

  int digits = 1;
  int tmpValue = value;
  while ( tmpValue /= 10 )
    digits++;
  // Serial.println(digits);
  unsigned char payload[digits];
  int packetLength = 0;

  //dla statusu potencjometru i lampki, to samo - > odesłanie value w payload
  sprintf(payload, "%u", value); //kopiowanie  value do payloadu


  setResponseMessageFields(responseMessage, coapMessage, messageID, payload, digits); //SPRAWIDZC CZY DOBRZE TE REFERENCJE SĄ PRZEKAZYWANE I OBIEKTY/TABLICE DO FUNKCJI :)

  unsigned char * packet = responseMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc
  // Serial.print("handleRadioRequest packetLen: ");
  // Serial.println(packetLength);
  sendUdpResponse(coapMessage, packet, packetLength);
  delete packet;
}

void handleRadioRequest(short option, short value)    //przeciążenie do obsługi zasobów o dużym czasie dostępu
{
  for (int i = 0; i < 10; i++) {
    if (observersList[i].isEmpty == false) {

      observersList[i].sequenceNumber++;

      CoapMessage observeMessage;

      uint8_t messageID[] = {201, 201};

      int digits = 1;
      int tmpValue = value;

      while ( tmpValue /= 10 )
        digits++;

      unsigned char payload[digits];
      int packetLength = 0;

      //dla statusu potencjometru i lampki, to samo - > odesłanie value w payload
      sprintf(payload, "%u", value); //kopiowanie  value do payloadu

      observeMessage.setHeader(observersList[i].token, observersList[i].tokenLength, CoapUtils::MessageType::CON, CoapUtils::ResponseCode::SUCCESS, CoapUtils::SuccessResponseCode::CONTENT, messageID);
      observeMessage.setRemoteIPAddress(observersList[i].remoteAddress);
      observeMessage.setRemotePort(observersList[i].remotePort);
      observeMessage.setContentFormat(0);
      observeMessage.setPayload(payload, digits);

      uint8_t observeValue[3];
      observeValue[2] = (uint8_t)(observersList[i].sequenceNumber >> 0);
      observeValue[1] = (uint8_t)(observersList[i].sequenceNumber >> 8);
      observeValue[0] = (uint8_t)(observersList[i].sequenceNumber >> 16);

      observeMessage.setObserveValue(true, observeValue);

      unsigned char * packet = observeMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc
      sendUdpResponse(observeMessage, packet, packetLength);
      sendCon++;

      previousTime = millis();
      unsigned long timeout = random(ACK_TIMEOUT * 1000, ACK_TIMEOUT * ACK_RANDOM_FACTOR * 1000);
      int repeats = 0;

      while (true) {

        currentTime = millis();
        if (currentTime - previousTime >= timeout) {
          timeout *= 2;
          repeats++;
          previousTime = currentTime;
          sendUdpResponse(observeMessage, packet, packetLength);
          sendCon++;
        }

        if (repeats > MAX_RETRANSMIT) {
          Serial.println("Nie otrzymano wiadomosc ACK, usuwam obserwatora");
          removeObserverForPotStatus(observeMessage);
          break;
        }
        int ackPacketSize = Udp.parsePacket();

        if (ackPacketSize) {
          CoapMessage ackCoapMessage;
          unsigned char * ackPacket = new unsigned char[ackPacketSize];

          Udp.read(packetBuffer, MAX_BUFFER); //wczytanie pakietu do bufora
          memcpy(ackPacket, packetBuffer, ackPacketSize);  //skopiowanie do zmiennej packet

          ackCoapMessage.parse(ackPacket, ackPacketSize, Udp.remoteIP(), Udp.remotePort());// "wypełnienie obiektu CoapMessage" danymi pakietu
          delete ackPacket;

          if (ackCoapMessage.getMessageType() == CoapUtils::MessageType::ACK) {
            Serial.println("Otrzymano wiadomosc ACK");
            recAck++;
            break;
          }
        }
      }
      delete packet;

    }
  }
}

void sendUdpResponse(CoapMessage & coapMessage, unsigned char * packet, int packetLength) {
  Serial.println("sendUdpResponse");
  Udp.beginPacket(coapMessage.getRemoteIPAddress(), coapMessage.getRemotePort());
  Udp.write(packet, packetLength);
  Udp.endPacket();
}

void setResponseMessageFields(CoapMessage & responseMessage,  CoapMessage & coapMessage, uint8_t messageID[], unsigned char * payload, int payloadLength) {
  if (payload == NULL)
  {
    // Serial.println("payload null");
    return;
  }
  else
  {
    // Serial.println("payload nie null");

    responseMessage.setHeader(coapMessage.getToken(), coapMessage.getTokenLength(), CoapUtils::MessageType::NON, CoapUtils::ResponseCode::SUCCESS, CoapUtils::SuccessResponseCode::CONTENT, messageID);
    responseMessage.setContentFormat(0);
    responseMessage.setPayload(payload, payloadLength);
    //   showDebug(coapMessage);

    //   showDebug(responseMessage);
  }
}


void sendAckToCon(String value, CoapMessage &message){
 Serial.println(value);
  CoapMessage responseMessage;
  uint8_t messageID[] = {201, 201};

 int payloadLength = value.length();
  unsigned char payload[payloadLength];
  int packetLength = 0;

value.toCharArray(payload, payloadLength);

  setResponseMessageFields(responseMessage, message, messageID, payload, payloadLength
  
  ); //SPRAWIDZC CZY DOBRZE TE REFERENCJE SĄ PRZEKAZYWANE I OBIEKTY/TABLICE DO FUNKCJI :)

  unsigned char * packet = responseMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc
  // Serial.print("handleRadioRequest packetLen: ");
  // Serial.println(packetLength);
  sendUdpResponse(message, packet, packetLength);
  delete packet;
}


