#include "SwitecX25.h"

// Define the pin connections for the rotary encoder
#define BUTTON_PIN 2 // SW -> D2
#define OUTPUT_A 3 // CLK -> D3
#define OUTPUT_B 4 // DT-> D4

#define BUTTON_BOUNCE_THRESHOLD 200
#define ROTARY_BOUNCE_THRESHOLD 20

int counter = 0; //Define the count

volatile bool buttonPushed = false;
volatile bool aTriggered = false;

unsigned long timestamp;

unsigned long lastButtonPush = 0;
unsigned long lastATrigger = 0;

int numSteps = 315*3;
int speed = 15;
int max = numSteps / speed;
SwitecX25 motor(numSteps,8,9,10,11);

void setup() {
  // enable the button press
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // set the pins for the rotary encoder
  pinMode(OUTPUT_A, INPUT);
  pinMode(OUTPUT_B, INPUT);

  // This is when the button is pushed
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ButtonPushed, FALLING);
  attachInterrupt(digitalPinToInterrupt(OUTPUT_A), OutputAChanged, FALLING);

  Serial.begin(9600);
  motor.zero();
  //motor.setPosition(427);
}

void loop() {
  if (buttonPushed)
  {
    timestamp = millis();
    if (timestamp - lastButtonPush > BUTTON_BOUNCE_THRESHOLD)
    {
      lastButtonPush = timestamp;
      FullSweep();
      Serial.println("Button Pressed!");
    }
    buttonPushed = false;
  }
  else if (aTriggered)
  {
    timestamp = millis();
    if (timestamp - lastATrigger > ROTARY_BOUNCE_THRESHOLD)
    {
      lastATrigger = timestamp;
      if (digitalRead(OUTPUT_B) == 1) {
        if(counter > 0) {
          counter--;
          Serial.print("CCW: ");
        }
      }
      if (digitalRead(OUTPUT_B) == 0) {
        if (counter < max) {
          counter++;
          Serial.print("CW: ");
        }
        
      }
      Serial.println(counter);
    }
    motor.setPosition(counter * speed);
    aTriggered = false;
  }
  motor.update();

}

//The interrupt handlers
void ButtonPushed()
{
  buttonPushed = true;
}

void OutputAChanged()
{
  aTriggered = true;
}

void FullSweep()
{
  counter = 0;
  motor.zero();
  motor.setPosition(numSteps);
  motor.updateBlocking();
  motor.zero();
}