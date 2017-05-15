#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>


RF24 radio(7, 8);
RF24Network network(radio);

uint8_t OUR_CHANNEL = 120;
uint8_t THIS_NODE = 01;

short isLampOn = 0;
short previousPotValue = 0;
short currentPotValue = 0;


enum option {LampOff, LampOn, LampStatus, PotStatus};


struct Request {
  short option;             //0 - lampka off, 1 - lampka ON, 2 - stan lampki, 3 - stan potencjometru
};


struct Response {
  short option;             //0 - lampka off, 1 - lampka ON, 2 - stan lampki, 3 - stan potencjometru
  short value;
};

void setup() {
  Serial.begin(57600);
  SPI.begin();
  radio.begin();
  network.begin(OUR_CHANNEL, THIS_NODE);
  pinMode(3, OUTPUT);
}


void loop() {

  network.update();

  // Nasłuchuje na wiadomości od CoAP-server
  while (network.available()) {
    struct Request message;
    RF24NetworkHeader header;
    network.read(header, &message, sizeof(struct Request));
    Serial.println(message.option, DEC);
    handleRequest(message.option);
  }

currentPotValue = analogRead(0);
  if(currentPotValue != previousPotValue)
  {
  sendResponse(PotStatus, currentPotValue);
  previousPotValue = currentPotValue;
  }

  
  
}

void handleRequest(short option) {
  if (option == LampOff) {
    analogWrite(3, 255);
    isLampOn = 0;
  }

  if (option == LampOn) {
    analogWrite(3, 0);
    isLampOn = 1;
  }

  if (option == LampStatus) {
    sendResponse(LampStatus, isLampOn);
  }

  if (option == PotStatus) {
    short potentiometerValue = analogRead(0);
    Serial.println(potentiometerValue, DEC);
    sendResponse(PotStatus, potentiometerValue);
  }
}

void sendResponse(short option, short value) {
  RF24NetworkHeader header(00);
  struct Response response;

  response.option = option;
  response.value = value;

  network.write(header, &response, sizeof(response));
  Serial.println("wysłano");
}




