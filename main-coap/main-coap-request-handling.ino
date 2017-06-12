
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

  showDebug(coapMessage); //in main-coap file :)
  debugPayload(payload, coapMessage.getPayloadLength());

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

  debugPayload(payloadCharArray, sizeof(payloadCharArray));  //from main-coap.ino
  debugPayload(responseMessage.getPayload(), responseMessage.getPayloadLength());

  responseMessage.setPayload(payloadCharArray, sizeof(payloadCharArray));

  packet = responseMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc
  sendUdpResponse(coapMessage, packet, packetLength);
  delete packet;

}

//czeka na odpowiedź od resourca i przekazuje ją do funkcji ją obsługującej
void waitForResponseAndHandleIt(CoapMessage &coapMessage) {
  boolean waitForResponse = true;

  while (waitForResponse) {
    Serial.println("przyszla odpowiedz z resourca");
    network.update();
    while (network.available()) {
      Serial.println("Odebrano");
      struct Response message;
      RF24NetworkHeader header;
      network.read(header, &message, sizeof(struct Response));
      handleRadioRequest(message.option, message.value, coapMessage);
      waitForResponse = false;
    }

  }
}

void addObserverForPotStatus(CoapMessage &coapMessage) {
  Serial.println("W observe 0");
  observersList[0].isEmpty = false;
  observersList[0].remoteAddress = coapMessage.getRemoteIPAddress();
  unsigned char * oToken;

  memcpy(&oToken, coapMessage.getToken(), coapMessage.getTokenLength());
  observersList[0].token = oToken;

  sendRequestViaRadio(PotStatus);
  Serial.println("Odpowiedz ze statusem potencjometru");
  waitForResponseAndHandleIt(coapMessage);

}

void removeObserverForPotStatus(CoapMessage &coapMessage) {
  //todo
}




