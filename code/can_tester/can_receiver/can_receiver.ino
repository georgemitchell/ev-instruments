#include <SPI.h>
#include <mcp2515.h>

// Define the pin connections for the CAN 2515 module
#define CAN_RECV_INT_PIN 2
#define CAN_SCK_PIN 13
#define CAN_SI_PIN 11
#define CAN_SO_PIN 12
#define CAN_RECV_CS_PIN 10


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

struct can_frame canMessageRecv;

MCP2515 recvBus(CAN_RECV_CS_PIN);

void setup() {
  // put your setup code here, to run once:
  while (!Serial);
  Serial.begin(9600);
  SPI.begin();
  //Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(CAN_RECV_INT_PIN), MessageReceived, CHANGE);

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
}

void MessageReceived()
{
  messageReceived = true;
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
