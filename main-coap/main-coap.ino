#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <CoapMessage.h>
#include <CoapUtils.h>
#include <DebugUtils.h>

#include "CoapResource.h"

#define MAX_BUFFER 60



byte mac[] = {0x00, 0xaa, 0xbb, 0xcc, 0xde, 0xf8};
byte ip [] = {192, 168, 0, 25};
EthernetUDP Udp;
short localPort = 5683;

char packetBuffer[MAX_BUFFER];

RF24 radio(7, 8);
RF24Network network(radio);






uint8_t OUR_CHANNEL = 120;
uint8_t THIS_NODE = 00;




enum option {LampOff, LampOn, LampStatus, PotStatus};


struct Request {
  short option;             //0 - lampka off, 1 - lampka ON, 2 - stan lampki, 3 - stan potencjometru
};


struct Response {
  short option;             //0 - lampka off, 1 - lampka ON, 2 - stan lampki, 3 - stan potencjometru
  short value;
};

 struct observer
{
  bool isEmpty;
  IPAddress remoteAddress;
  unsigned char * token;
};

  struct observer observersList[10];            // 10 observerów może być

void setup() {
  Serial.begin(115200);
  SPI.begin();
  radio.begin();
  network.begin(OUR_CHANNEL, THIS_NODE);
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());
  Udp.begin(localPort);
  initializeObservers(observersList);
}






void loop() {



  //==============UDP COAP COMMUNICATION =================
  //nasłuchuje na pakiety udp coapa
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.flush();
    // Serial.println("Jestem w  , packetSize=");
    debugVar(&packetSize);
    // Serial.println(packetSize);
    Udp.read(packetBuffer, MAX_BUFFER);
    //   sendRequestViaRadio(packetBuffer[0] - '0');

    //1. parsowanie wiadomosci COAP. (sprawdzenie czy poprawna struktura wiadomosci, payload, nagłówek, opcje)
    //2. reakcja zależna od wiadomosci - ogólnie komunikacja z lampką drogą radiową, pobranie danych, zmiana statusu lamplki itp
    //3. wysłanie poprawnej odpowiedzi coap - uprzednie stworzenie, headery itp itp
    //zapis tokena dla wiadomosci, bo musi  byc w odpowiedzi taki sam

    unsigned char * packet = new unsigned char[packetSize];
    memcpy(packet, packetBuffer, packetSize);
    hexPacket(packet, packetSize);
    CoapMessage coapMessage;
    coapMessage.parse(packet, packetSize, Udp.remoteIP(), Udp.remotePort());
    delete packet;
    //  Serial.println(packet[0]);
    //  Serial.println(packet[1]);
    //  Serial.println(packet[2]);
    //  Serial.println(packet[3]);

    //handleGetRequest(coapMessage);
    if (coapMessage.getCodeDetails() == CoapUtils::RequestMethod::GET) {
      handleGetRequest(coapMessage);
      //    Serial.println("wyszedlem z handleReq");
    }

    if (coapMessage.getCodeDetails() == CoapUtils::RequestMethod::PUT) {
      handlePutRequest(coapMessage);
    }

  }




  //===========RADIO COMMUNICATION =======================
  // Nasłuchuje na wiadomości radiowe od CoAP-resource (poziom potencjometru, status lampki)
  network.update();
  while (network.available()) {
    //   Serial.println("Odebrano");
    struct Response message;
    RF24NetworkHeader header;
    network.read(header, &message, sizeof(struct Response));
    //   Serial.println(message.option, DEC);
    handleRadioRequest(message.option, message.value);
  }
}



//=========FUNCTIONS =================

//=======RADIO=======
void handleRadioRequest(short option, short value, CoapMessage &coapMessage) {     //dla zasobów o krótkim czasie dostępu, trochę na łatwiznę

  if (option == LampStatus) {
    Serial.println(value);
    CoapMessage responseMessage;
    uint8_t messageID[] = {201, 201};
    responseMessage.setHeader(coapMessage.getToken(), coapMessage.getTokenLength(), CoapUtils::MessageType::NON, CoapUtils::ResponseCode::SUCCESS, CoapUtils::SuccessResponseCode::CONTENT, messageID);
    responseMessage.setContentFormat(0);

    unsigned char payload[2];
    sprintf(payload, "%u", value);

    responseMessage.setPayload(payload, sizeof(payload));

    int packetLength = 0;
    unsigned char * packet = responseMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc

    Udp.beginPacket(coapMessage.getRemoteIPAddress(), coapMessage.getRemotePort());
    Udp.write(packet, packetLength);
    Udp.endPacket();

    delete packet;
  }

  if (option == PotStatus) {
    Serial.print("Pot value=");
    Serial.println(value);
    CoapMessage responseMessage;
    uint8_t messageID[] = {201, 201};
    responseMessage.setHeader(coapMessage.getToken(), coapMessage.getTokenLength(), CoapUtils::MessageType::NON, CoapUtils::ResponseCode::SUCCESS, CoapUtils::SuccessResponseCode::CONTENT, messageID);
    responseMessage.setContentFormat(0);

    unsigned char* payload;
    //sprintf(payload, "%u", value);
    memcpy(&payload,&value,sizeof(value));

 //  payload = (unsigned char* )&value;

  Serial.println(payload[0]);
  Serial.println(payload[1]);
  Serial.println();

    responseMessage.setPayload(payload, sizeof(payload));

    int packetLength = 0;
    unsigned char * packet = responseMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc

    Udp.beginPacket(coapMessage.getRemoteIPAddress(), coapMessage.getRemotePort());
    Udp.write(packet, packetLength);
    Udp.endPacket();

    delete packet;
  }
}

void handleRadioRequest(short option, short value)    //przeciążenie do obsługi zasobów o dużym czasie dostępu
{

}

void sendRequestViaRadio(short option) {
  RF24NetworkHeader header(01);
  struct Request request;

  request.option = option;

  network.write(header, &request, sizeof(request));
}


//======COAP==========
void handleGetRequest(CoapMessage &coapMessage) {

  String uriPath;
  coapMessage.getUriPath(uriPath);
  //  showDebug(coapMessage);
  Serial.print("UriPath=");
  Serial.println(uriPath);


  // debugVar(uriPath);
  if (uriPath == ".well-known/core") {
    Serial.println("odkrywanie zasobow");
    CoapMessage responseMessage;
    uint16_t messageID = 3124;
    responseMessage.setHeader(coapMessage.getToken(), coapMessage.getTokenLength(), CoapUtils::MessageType::NON, CoapUtils::ResponseCode::SUCCESS, CoapUtils::SuccessResponseCode::CONTENT, messageID);
    responseMessage.setContentFormat(40);

    // zasoby
    String ledLamp = "</led>;rt=\"led-lamp-status\";if=\"executable\";ct=0,";
    String potentiometer = "</potentiometer>;rt=\"led-lamp-status\";if=\"executable\";ct=0;obs,";

    //   ledLamp.concat(potentiometer);

    // String payload;

    Serial.println(ledLamp);
    //Serial.flush();
    unsigned char payloadCharArray[ledLamp.length() + 1];
    ledLamp.toCharArray((char*)payloadCharArray, ledLamp.length() + 1);


    Serial.println("Payload");

    for (int i = 0; i < sizeof(payloadCharArray); i++)
    {
      Serial.print((char)payloadCharArray[i]);
    }
    Serial.println();
    // Serial.flush();
    responseMessage.setPayload(payloadCharArray, sizeof(payloadCharArray));

    //  Serial.flush();

    Serial.println("Payload response");
    for (int i = 0; i < responseMessage.getPayloadLength(); i++)
    {
      Serial.print((char)responseMessage.getPayload()[i]);
    }
    int packetLength = 0;
    unsigned char * packet = responseMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc

    Udp.beginPacket(coapMessage.getRemoteIPAddress(), coapMessage.getRemotePort());
    Udp.write(packet, packetLength);
    Udp.endPacket();

    delete packet;
  }

  if (uriPath == "Lampka")
  {
    sendRequestViaRadio(LampStatus);
    Serial.println("Odpowiedz ze statusem lampki");
    boolean goOut = false;
    while (goOut != true)
    {
      Serial.println("przyszla odpowiedz");
      network.update();
      while (network.available()) {
        //   Serial.println("Odebrano");
        struct Response message;
        RF24NetworkHeader header;
        network.read(header, &message, sizeof(struct Response));
        handleRadioRequest(message.option, message.value, coapMessage);
      }
      goOut = true;
    }
  }


  if (uriPath == "Potencjometr")
  {
    uint8_t observeOption = coapMessage.getObserve();
    Serial.print("ObserveOption=");
    Serial.println(observeOption);
    
   
      if (observeOption == 0)                                   // dodawanie do listy obserwatorow
      {
        Serial.println("W observe 0");
        observersList[0].isEmpty = false;
        observersList[0].remoteAddress = coapMessage.getRemoteIPAddress();
        unsigned char * oToken;
        memcpy(&oToken, coapMessage.getToken(),coapMessage.getTokenLength());
        observersList[0].token = oToken;
       
        sendRequestViaRadio(PotStatus);
        Serial.println("Odpowiedz ze statusem potencjometru");
        boolean goOut = false;
        while (goOut != true)
        {
          Serial.println("przyszla odpowiedz");
          network.update();
          while (network.available()) {
            //   Serial.println("Odebrano");
            struct Response message;
            RF24NetworkHeader header;
            network.read(header, &message, sizeof(struct Response));
            handleRadioRequest(message.option, message.value, coapMessage);
          }
          goOut = true;
        }
      }
      else if (observeOption == 1)                              //  usuwanie z listy obserwatorow
      {
      }
    
    else
    {
      sendRequestViaRadio(PotStatus);
        Serial.println("Odpowiedz z wartoscia ");
        boolean goOut = false;
        while (goOut != true)
        {
          Serial.println("przyszla odpowiedz");
          network.update();
          while (network.available()) {
            //   Serial.println("Odebrano");
            struct Response message;
            RF24NetworkHeader header;
            network.read(header, &message, sizeof(struct Response));
            handleRadioRequest(message.option, message.value, coapMessage);
          }
          goOut = true;
        }
    }
  }
}



void handlePutRequest(CoapMessage &coapMessage) {
  showDebug(coapMessage);
  Serial.print("payload=");
  unsigned char* payload = coapMessage.getPayload();
  for (int i = 0; i < coapMessage.getPayloadLength(); i++)
  {
    Serial.print((char)payload[i]);
  }

  sendRequestViaRadio((short)(payload[0] - '0'));

}

void showDebug(CoapMessage &coapMessage) {
  debugVar(coapMessage.getCoapVersion());
  debugVar(coapMessage.getCoapVersion());
  debugVar(coapMessage.getMessageType());
  debugVar(coapMessage.getTokenLength());
  debugVar(coapMessage.getCodeClass());
  debugVar(coapMessage.getCodeDetails());

  //  Serial.print("token=");
  unsigned char* token = coapMessage.getToken();
  for (int i = 0; i < coapMessage.getTokenLength(); i++)
  {
    Serial.println(token[i], HEX);
  }

  String uriPath;
  coapMessage.getUriPath(uriPath);
  Serial.print("UriPath = ");
  Serial.println(uriPath);
  Serial.println("Koniec showDebug");

}

void hexPacket(unsigned char * packet, int packetLength) {
  Serial.println("HEX PACKET");
  for (int i = 0; i < packetLength; i++) {
    Serial.print(packet[i], HEX);
    Serial.print(", ");
    if (!(i % 10)) Serial.println();
  }
}

void initializeObservers(struct observer observers[])
{
  for(int i=0; i<10;i++)
  {
  
    observers[i].isEmpty = true;
    observers[i].remoteAddress = IPAddress(0,0,0,0);
    observers[i].token = nullptr;
  }
}

