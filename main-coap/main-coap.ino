#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define MAX_BUFFER 60



byte mac[] = {0x00, 0xaa, 0xbb, 0xcc, 0xde, 0xf80};

EthernetUDP Udp;
short localPort = 1242;

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

int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print("Jestem w if, packetSize=");
    Serial.println(packetSize);

    Udp.read(packetBuffer, MAX_BUFFER);

    sendRequest(packetBuffer[0]-'0');
  }
 
  network.update();

  // Nasłuchuje na wiadomości od CoAP-resource
  while (network.available()) {
    Serial.println("Odebrano");
    struct Response message;
    RF24NetworkHeader header;
    network.read(header, &message, sizeof(struct Response));
    Serial.println(message.option, DEC);
    handleRequest(message.option, message.value);
  }
}

void handleRequest(short option, short value) {

  if (option == LampStatus) {
    Serial.println(value);
  }

  if (option == PotStatus) {
    Serial.println(value);
  }
}

void sendRequest(short option) {
  RF24NetworkHeader header(01);
  struct Request request;

  request.option = option;

  network.write(header, &request, sizeof(request));
}




