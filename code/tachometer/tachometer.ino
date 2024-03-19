#include <SPI.h>
#include <mcp2515.h>
#include "SwitecX25.h"

// Define the data for the gauge switch
#define TOGGLE_PIN 2
#define BOUNCE_THRESHOLD 100

// Define the pin connections for the CAN 2515 module
#define CAN_INT_PIN 2
#define CAN_SCK_PIN 13
#define CAN_SI_PIN 11
#define CAN_SO_PIN 12
#define CAN_CS_PIN 10

// Define the pin connections for the x27 168 stepper motor
#define LOWER_LEFT_PIN 4
#define UPPER_LEFT_PIN 5
#define LOWER_RIGHT_PIN 6
#define UPPER_RIGHT_PIN 7

// Define the CAN IDs
#define RPM_CAN_ID 0x0A
#define POWER_CAN_ID 0x0B

#define RPM_MIN 0
#define RPM_MAX 8000

#define POWER_MIN 0
#define POWER_MAX 5000

volatile bool toggled = false;

unsigned long timestamp;
unsigned long lastToggle = 0;

int switchState = -1;
int currentSwitchState = 0;

int canToRead = RPM_CAN_ID;

volatile bool messageReceived = false;

int numSteps = 315*3;
SwitecX25 motor(numSteps, LOWER_LEFT_PIN, UPPER_LEFT_PIN, LOWER_RIGHT_PIN, UPPER_RIGHT_PIN);

int rpmScale = (RPM_MAX - RPM_MIN) / numSteps;
int powerScale = (POWER_MAX - POWER_MIN) / numSteps; 

struct can_frame canMessage;
MCP2515 canBus(CAN_CS_PIN);

void setup() {
  while (!Serial);
  Serial.begin(9600);
  SPI.begin();
  
  attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), IncomingMessage, CHANGE);

  canBus.reset();
  canBus.setBitrate(CAN_500KBPS, MCP_8MHZ);
  canBus.setNormalMode();

  FullSweep();
  Serial.println("Ready to receive");
}

void loop() {
  // word (high, low);
  // https://forum.arduino.cc/t/integer-to-byte-array/41699/2

  // Check whether we have RPM or Current
  currentSwitchState = digitalRead(TOGGLE_PIN);
  if (currentSwitchState != switchState) {
    switchState = currentSwitchState;
    UpdateState();
  }

  if(messageReceived) {
    messageReceived = false;
    uint8_t irq = canBus.getInterrupts();
    if (irq) {
      ProcessMessage();
    }
  }
}

void IncomingMessage() {
  messageReceived = true;
}

void ProcessMessage() {
  unsigned int value;
  if (canBus.readMessage(&canMessage) == MCP2515::ERROR_OK) {
    if(canMessage.can_id == canToRead) {
      if(canMessage.can_dlc < 8) {
        Serial.println("Incorrect number of bytes in message");
      } else {
        value = word(canMessage.data[5], canMessage.data[4]);
        Serial.print("CAN message received: ");
        Serial.println(value);
        MoveNeedle(value);
      }
    }   
  }
}

void MoveNeedle(unsigned int value) {
  int position = 0;
  switch(canToRead) {
    case RPM_CAN_ID:
      position = value / rpmScale;
      break;

    case POWER_CAN_ID:
      position = value / powerScale;
      break;

    default:
      Serial.print("Unexpected CAN value sent: ");
      Serial.println(value);
      return;
  }
  motor.setPosition(position);
}

void UpdateState()
{
  /*if (switchState == HIGH) {
    canToRead = RPM_CAN_ID;
    Serial.println("Toggle is set to RPM");
  } else {
    canToRead = POWER_CAN_ID;
    Serial.println("Toggle is set to POWER");
  }*/
}

void FullSweep()
{
  motor.zero();
  motor.setPosition(numSteps);
  motor.updateBlocking();
  motor.zero();
}