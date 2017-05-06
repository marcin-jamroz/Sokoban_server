#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <CoapMessage.h>
#include <CoapUtils.h>

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
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());
  Udp.begin(localPort);

}






void loop() {


  //==============UDP COAP COMMUNICATION =================
  //nasłuchuje na pakiety udp coapa
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print("Jestem w  , packetSize=");
    Serial.println(packetSize);
    Udp.read(packetBuffer, MAX_BUFFER);
    //   sendRequestViaRadio(packetBuffer[0] - '0');

    //1. parsowanie wiadomosci COAP. (sprawdzenie czy poprawna struktura wiadomosci, payload, nagłówek, opcje)
    //2. reakcja zależna od wiadomosci - ogólnie komunikacja z lampką drogą radiową, pobranie danych, zmiana statusu lamplki itp
    //3. wysłanie poprawnej odpowiedzi coap - uprzednie stworzenie, headery itp itp
    //zapis tokena dla wiadomosci, bo musi  byc w odpowiedzi taki sam

    unsigned char* packet = new unsigned char[packetSize];
    memcpy(packet, packetBuffer, packetSize);
    CoapMessage coapMessage;
    coapMessage.parse(packet, packetSize);
    Serial.println(packet[0]);
    Serial.println(packet[1]);
    Serial.println(packet[2]);
    Serial.println(packet[3]);

    //handleGetRequest(coapMessage);
    if (coapMessage.getCodeDetails() == CoapUtils::RequestMethod::GET) {
      handleGetRequest(coapMessage);
      Serial.flush();
      delay(100);
      Serial.println("wyszedlem z handleReq");
    }
    //
    //    if (coapMessage.getMessageType() == CoapUtils::RequestMethod::PUT) {
    //      handlePutRequest(coapMessage);
    //    }

  }




  //===========RADIO COMMUNICATION =======================
  // Nasłuchuje na wiadomości radiowe od CoAP-resource (poziom potencjometru, status lampki)
  network.update();
  while (network.available()) {
    Serial.println("Odebrano");
    struct Response message;
    RF24NetworkHeader header;
    network.read(header, &message, sizeof(struct Response));
    Serial.println(message.option, DEC);
    handleRadioRequest(message.option, message.value);
  }
}



//=========FUNCTIONS =================

//=======RADIO=======
void handleRadioRequest(short option, short value) {

  if (option == LampStatus) {
    Serial.println(value);
  }

  if (option == PotStatus) {
    Serial.println(value);
  }
}

void sendRequestViaRadio(short option) {
  RF24NetworkHeader header(01);
  struct Request request;

  request.option = option;

  network.write(header, &request, sizeof(request));
}


//======COAP==========
void handleGetRequest(CoapMessage &coapMessage) {
  showDebug(coapMessage);
    Serial.println("Koniec handleReq");
}

void handlePutRequest(CoapMessage coapMessage) {

}

void showDebug(CoapMessage &coapMessage) {
  Serial.println(coapMessage.getCoapVersion());
  Serial.println(coapMessage.getMessageType());
  Serial.println(coapMessage.getTokenLength());
  Serial.println(coapMessage.getCodeClass());
  Serial.println(coapMessage.getCodeDetails());

  Serial.print("token=");
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




