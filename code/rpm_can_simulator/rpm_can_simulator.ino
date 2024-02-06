#include <SPI.h>
#include <mcp2515.h>

// Define the pin connections for the CAN 2515 module
#define CAN_INT_PIN 2
#define CAN_SCK_PIN 13
#define CAN_SI_PIN 11
#define CAN_SO_PIN 12
#define CAN_CS_PIN 10

// Define the pin connections for the rotary encoder
// (For this test we're not using the button)
#define OUTPUT_A 3 // CLK -> D3
#define OUTPUT_B 4 // DT-> D4
#define BOUNCE_THRESHOLD 20

// Define the RPM constants
#define RPM_MULTIPLIER 100
#define MAX_RPM 8000
#define RPM_CAN_ID 0x0A

volatile bool aTriggered = false;

unsigned long timestamp;
unsigned long lastATrigger = 0;

int counter = 0;
int max = MAX_RPM / RPM_MULTIPLIER;
unsigned int rpm = 0;


struct can_frame canMessage;

MCP2515 canBus(CAN_CS_PIN);

void setup() {
  // put your setup code here, to run once:
  while (!Serial);
  Serial.begin(9600);
  SPI.begin();

  canMessage.can_id = RPM_CAN_ID;
  canMessage.can_dlc = 8;

  // See RPM mesage flags here:
  // https://www.diyelectriccar.com/threads/rpm-data-from-hyper9-via-can.208774/

  // System Flags word
  canMessage.data[0] = 0x00;
  canMessage.data[1] = 0x00;

  // Motor Flags word
  canMessage.data[2] = 0x00;
  canMessage.data[3] = 0x00;

  // Speed 
  canMessage.data[4] = 0x00;
  canMessage.data[5] = 0x00;

  // Motor Throttle Request
  canMessage.data[6] = 0x00;
  
  // Motor Throttle
  canMessage.data[7] = 0x00;

  attachInterrupt(digitalPinToInterrupt(OUTPUT_A), OutputAChanged, FALLING);

  canBus.reset();
  canBus.setBitrate(CAN_500KBPS, MCP_8MHZ);
  canBus.setNormalMode();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (aTriggered)
  {
    timestamp = millis();
    if (timestamp - lastATrigger > BOUNCE_THRESHOLD)
    {
      lastATrigger = timestamp;
      if (digitalRead(OUTPUT_B) == 1) {
        if(counter > 0) {
          counter--;
          SendRPM();
        }
      }
      if (digitalRead(OUTPUT_B) == 0) {
        if (counter < max) {
          counter++;
          SendRPM();
        } 
      }
    }
    aTriggered = false;
  }
}

void OutputAChanged()
{
  aTriggered = true;
}

void SendRPM() {
  rpm = counter * RPM_MULTIPLIER;
  canMessage.data[4] = lowByte(rpm);
  canMessage.data[5] = highByte(rpm);
  canBus.sendMessage(&canMessage);
  Serial.print("RPM sent in CAN: ");
  Serial.println(rpm);
}

