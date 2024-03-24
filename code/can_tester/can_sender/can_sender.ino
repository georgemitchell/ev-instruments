#include <SPI.h>
#include <mcp2515.h>

// Define the pin connections for the CAN 2515 module
#define CAN_SEND_INT_PIN 2
#define CAN_SCK_PIN 13
#define CAN_SI_PIN 11
#define CAN_SO_PIN 12
#define CAN_SEND_CS_PIN 10


// Define the RPM constants
#define RPM_MULTIPLIER 100
#define MAX_RPM 8000
#define RPM_CAN_ID 0x0A

volatile bool messageReceived = false;

unsigned long timestamp;
unsigned long lastATrigger = 0;

int counter = 0;
int max = MAX_RPM / RPM_MULTIPLIER;
unsigned int rpm = 0;
unsigned int sentRpm = 0;


struct can_frame canMessageSend;

MCP2515 sendBus(CAN_SEND_CS_PIN);

void setup() {
  // put your setup code here, to run once:
  while (!Serial);
  Serial.begin(9600);
  SPI.begin();

  canMessageSend.can_id = RPM_CAN_ID;
  canMessageSend.can_dlc = 8;

  // See RPM mesage flags here:
  // https://www.diyelectriccar.com/threads/rpm-data-from-hyper9-via-can.208774/

  // System Flags word
  canMessageSend.data[0] = 0x00;
  canMessageSend.data[1] = 0x00;

  // Motor Flags word
  canMessageSend.data[2] = 0x00;
  canMessageSend.data[3] = 0x00;

  // Speed 
  canMessageSend.data[4] = 0x00;
  canMessageSend.data[5] = 0x00;

  // Motor Throttle Request
  canMessageSend.data[6] = 0x00;
  
  // Motor Throttle
  canMessageSend.data[7] = 0x00;

  sendBus.reset();
  sendBus.setBitrate(CAN_500KBPS, MCP_8MHZ);
  sendBus.setNormalMode();

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);

  counter = random(max);
  SendRPM();
}

void SendRPM() {
  rpm = counter * RPM_MULTIPLIER;
  canMessageSend.data[4] = lowByte(rpm);
  canMessageSend.data[5] = highByte(rpm);
  sendBus.sendMessage(&canMessageSend);
  Serial.print("RPM sent in CAN: ");
  Serial.println(rpm);
}

