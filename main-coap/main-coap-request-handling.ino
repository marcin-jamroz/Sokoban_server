
//======COAP - GET REQUEST HANDLING==========


void handleGetRequest(CoapMessage &coapMessage) {

  String uriPath;
  coapMessage.getUriPath(uriPath);
  //  showDebug(coapMessage);
  Serial.print("UriPath=");
  Serial.println(uriPath);
  // debugVar(uriPath);

  if (uriPath == ".well-known/core") {
    respondToWellKnownCoreGet(coapMessage);
  }

  if (uriPath == "Lampka") {
    sendRequestViaRadio(LampStatus);                        //wyślij geta po status lampki
    waitForResponseAndHandleIt(coapMessage);
  }

  if (uriPath == "Pot") {
    uint8_t observeOption = coapMessage.getObserve();
    Serial.print("ObserveOption=");
    Serial.println(observeOption);


    if (observeOption == 0) {                               // dodawanie do listy obserwatorow
      addObserverForPotStatus(coapMessage);
    }
    else if (observeOption == 1) {                           //  usuwanie z listy obserwatorow
      removeObserverForPotStatus(coapMessage);
    }
    else {
      sendRequestViaRadio(PotStatus);
      waitForResponseAndHandleIt(coapMessage);
    }
  }
}

void handlePutRequest(CoapMessage &coapMessage) {
  unsigned char* payload = coapMessage.getPayload();

//  showDebug(coapMessage); //in main-coap file :)
//  debugPayload(payload, coapMessage.getPayloadLength());

  sendRequestViaRadio((short)(payload[0] - '0'));  // W MAIN-COAP-RADIO-COMMUNICATION.INO FILE :)

}


void respondToWellKnownCoreGet(CoapMessage &coapMessage) {
  Serial.println("odkrywanie zasobow");
  CoapMessage responseMessage;
  uint16_t messageID = 3124;
  String ledLamp = "</Lampka>;rt=\"led-lamp-status\";if=\"executable\";ct=0,";                   //zasoby
  String potentiometer = "</potentiometer>;rt=\"led-lamp-status\";if=\"executable\";ct=0;obs,";
  unsigned char payloadCharArray[ledLamp.length() + 1];
  int packetLength = 0;
  unsigned char * packet;

  responseMessage.setHeader(coapMessage.getToken(), coapMessage.getTokenLength(), CoapUtils::MessageType::NON, CoapUtils::ResponseCode::SUCCESS, CoapUtils::SuccessResponseCode::CONTENT, messageID);
  responseMessage.setContentFormat(40);

  ledLamp.toCharArray((char*)payloadCharArray, ledLamp.length() + 1);

//  debugPayload(payloadCharArray, sizeof(payloadCharArray));  //from main-coap.ino
 // debugPayload(responseMessage.getPayload(), responseMessage.getPayloadLength());

  responseMessage.setPayload(payloadCharArray, sizeof(payloadCharArray));

  packet = responseMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc
  sendUdpResponse(coapMessage, packet, packetLength);
  delete packet;

}

//czeka na odpowiedź od resourca i przekazuje ją do funkcji ją obsługującej
void waitForResponseAndHandleIt(CoapMessage &coapMessage) {
  boolean waitForResponse = true;

  while (waitForResponse) {
    //Serial.println("przyszla odpowiedz z resourca");
    network.update();
    while (network.available()) {
      //Serial.println("Odebrano");
      struct Response message;
      RF24NetworkHeader header;
      network.read(header, &message, sizeof(struct Response));
      handleRadioRequest(message.option, message.value, coapMessage);
      waitForResponse = false;
    }



  }
}

void addObserverForPotStatus(CoapMessage &coapMessage) {

  for (int i = 0; i < 10; i++) {

    if (observersList[i].isEmpty == true) {
      Serial.println("W observe 0");
      observersList[i].isEmpty = false;
      observersList[i].remoteAddress = coapMessage.getRemoteIPAddress();
      observersList[i].remotePort = coapMessage.getRemotePort();
      unsigned char * oToken = new unsigned char[coapMessage.getTokenLength()];

      memcpy(oToken, coapMessage.getToken(), coapMessage.getTokenLength());
      observersList[i].token = oToken;

      Serial.println();
      Serial.print("Token observer: ");
      for (int i = 0; i < coapMessage.getTokenLength(); i++) {
        Serial.print(oToken[i], HEX);
      }

      Serial.println();

      observersList[i].tokenLength = coapMessage.getTokenLength();

      sendRequestViaRadio(PotStatus);
      Serial.println("Odpowiedz ze statusem potencjometru");
   //   waitForResponseAndHandleIt(coapMessage);

      break;
    }
  }
}

void removeObserverForPotStatus(CoapMessage &coapMessage) {
  for (int i = 0; i < 10; i++) {
    if (coapMessage.getTokenLength() == observersList[i].tokenLength) {
      boolean isEqual = true;
      for (int n = 0; n < coapMessage.getTokenLength(); n++) {
        if (coapMessage.getToken()[n] != observersList[i].token[n]) {
          isEqual = false;
        }
      }

      if (isEqual == true) {
        Serial.println("Usunieto obserwatora");
        observersList[i].isEmpty = true;
        observersList[i].remoteAddress = IPAddress(0, 0, 0, 0);
        delete observersList[i].token;
        observersList[i].token = nullptr;
        observersList[i].tokenLength = 0;
        observersList[i].remotePort = 0;
        observersList[i].sequenceNumber = 2;

        uint8_t zeroObserve[3] = {5, 5, 5};
        coapMessage.setObserveValue(false, zeroObserve);
        break;
      }
    }
  }
}




