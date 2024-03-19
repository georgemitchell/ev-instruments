#include <SPI.h>
#include <mcp2515.h>

// Define the pin connections for the CAN 2515 module
#define CAN_SEND_INT_PIN 2
#define CAN_RECV_INT_PIN 3
#define CAN_SCK_PIN 13
#define CAN_SI_PIN 11
#define CAN_SO_PIN 12
#define CAN_SEND_CS_PIN 10
#define CAN_RECV_CS_PIN 4


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
struct can_frame canMessageRecv;

MCP2515 sendBus(CAN_SEND_CS_PIN);
MCP2515 recvBus(CAN_RECV_CS_PIN);

void setup() {
  // put your setup code here, to run once:
  while (!Serial);
  Serial.begin(9600);
  SPI.begin();
  //Serial.begin(115200);

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

  attachInterrupt(digitalPinToInterrupt(CAN_RECV_INT_PIN), MessageReceived, CHANGE);

  sendBus.reset();
  sendBus.setBitrate(CAN_500KBPS, MCP_8MHZ);
  sendBus.setNormalMode();

  recvBus.reset();
  recvBus.setBitrate(CAN_500KBPS, MCP_8MHZ);
  recvBus.setNormalMode();

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);

  if(messageReceived) {
    messageReceived = false;
    uint8_t irq = recvBus.getInterrupts();
    if (irq) {
      if (recvBus.readMessage(&canMessageRecv) == MCP2515::ERROR_OK) {
        ReadRPM();
      }
    }
  }
  counter = random(max);
  SendRPM();
}

void MessageReceived()
{
  messageReceived = true;
}

void SendRPM() {
  rpm = counter * RPM_MULTIPLIER;
  canMessageSend.data[4] = lowByte(rpm);
  canMessageSend.data[5] = highByte(rpm);
  sendBus.sendMessage(&canMessageSend);
  Serial.print("RPM sent in CAN: ");
  Serial.println(rpm);
}

void ReadRPM() {
  if(canMessageRecv.can_id == RPM_CAN_ID) {
    if(canMessageRecv.can_dlc < 8) {
      Serial.println("Incorrect number of bytes in message");
    } else {
      sentRpm = word(canMessageRecv.data[5], canMessageRecv.data[4]);
      Serial.print("CAN message received: ");
      Serial.println(sentRpm);

    }
  }   
}
