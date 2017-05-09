#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <CoapMessage.h>
#include <CoapUtils.h>
#include <DebugUtils.h>

#define MAX_BUFFER 60



byte mac[] = {0x00, 0xaa, 0xbb, 0xcc, 0xde, 0xf8};
byte ip [] = {192, 168, 0, 200};
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

void setup() {
  Serial.begin(115200);
  SPI.begin();
  radio.begin();
  network.begin(OUR_CHANNEL, THIS_NODE);
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());
  Udp.begin(localPort);

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
void handleRadioRequest(short option, short value, CoapMessage coapMessage) {     //dla zasobów o krótkim czasie dostępu, trochę na łatwiznę

  if (option == LampStatus) {
    Serial.println(value);
    CoapMessage responseMessage;
    uint8_t messageID[] = {201, 201};
    responseMessage.setHeader(coapMessage.getToken(), coapMessage.getTokenLength(), CoapUtils::MessageType::NON, CoapUtils::ResponseCode::SUCCESS, CoapUtils::SuccessResponseCode::CONTENT, messageID);
    responseMessage.setContentFormat(0);
    
    unsigned char payload[] = "wartosc";
    
    responseMessage.setPayload(payload, sizeof(payload));

    int packetLength = 0;
    unsigned char * packet = responseMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc

    Udp.beginPacket(coapMessage.getRemoteIPAddress(), coapMessage.getRemotePort());
    Udp.write(packet, packetLength);
    Udp.endPacket();

    delete packet;
  }

  if (option == PotStatus) {
    Serial.println(value);
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


  // debugVar(uriPath);
  if (uriPath == ".well-known/core") {
    Serial.println("odkrywanie zasobow");
    CoapMessage responseMessage;
    uint8_t messageID[] = {200, 200};
    responseMessage.setHeader(coapMessage.getToken(), coapMessage.getTokenLength(), CoapUtils::MessageType::NON, CoapUtils::ResponseCode::SUCCESS, CoapUtils::SuccessResponseCode::CONTENT, messageID);
    responseMessage.setContentFormat(40);
    
    // zasoby
    CoapResource ledLamp = CoapResource("led", "led-lamp-status", "executable", 0, false);
    CoapResource potentiometer = CoapResource("potentiometer", "potentiometer-status", "sensor", 0, true);

    String resourcesLinks = String(ledLamp.getCoreLinkFormat() + potentiometer.getCoreLinkFormat());

    unsigned char payload[resourcesLinks.length()];
    resourcesLinks.toCharArray((char[])payload, resourcesLinks.length());

    responseMessage.setPayload(payload, sizeof(payload));

    int packetLength = 0;
    unsigned char * packet = responseMessage.toPacket(packetLength); // packetLength jest przekazywane przez referencję i jest zmieniane w funkcji na prawidlową wartosc

    Udp.beginPacket(coapMessage.getRemoteIPAddress(), coapMessage.getRemotePort());
    Udp.write(packet, packetLength);
    Udp.endPacket();

    delete packet;
  }

  if (uriPath == "Lampka")
  {
    sendRequestViaRadio(2);
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
