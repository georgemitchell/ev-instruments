#include <SPI.h>
#include <mcp2515.h>
#include "SwitecX25.h"

// Define the pin connections for the CAN 2515 module
#define CAN_INT_PIN 2
#define CAN_SCK_PIN 13
#define CAN_SI_PIN 11
#define CAN_SO_PIN 12
#define CAN_CS_PIN 10

struct can_frame canMessage;
MCP2515 canBus(CAN_CS_PIN);

void setup() {
  while (!Serial);
  Serial.begin(9600);
  SPI.begin();
  
  canBus.reset();
  canBus.setBitrate(CAN_500KBPS, MCP_8MHZ);
  canBus.setNormalMode();

  attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), IncomingMessage, CHANGE);

  Serial.println("hello");
}

void loop() {
  // word (high, low);
  // https://forum.arduino.cc/t/integer-to-byte-array/41699/2
  if (canBus.readMessage(&canMessage) == MCP2515::ERROR_OK) {
    Serial.print(canMessage.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMessage.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMessage.can_dlc; i++)  {  // print the data
      Serial.print(canMessage.data[i],HEX);
      Serial.print(" ");
    }

    Serial.println();      
  }
}

void IncomingMessage() {
  Serial.println("message received");
}