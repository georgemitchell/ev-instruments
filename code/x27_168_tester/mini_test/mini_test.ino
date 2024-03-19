#include "SwitecX25.h"

#define LOWER_LEFT 7
#define UPPER_LEFT 6
#define UPPER_RIGHT 4
#define LOWER_RIGHT 5

int numSteps = 315*3;
int speed = 15;
int max = numSteps / speed;
SwitecX25 motor(numSteps, LOWER_LEFT, UPPER_LEFT, LOWER_RIGHT, UPPER_RIGHT);

void setup() {

  Serial.begin(9600);
  motor.zero();
}

void loop() {
  delay(5000);
  FullSweep();
}

void FullSweep()
{
  //motor.zero();
  motor.setPosition(numSteps);
  motor.updateBlocking();
  motor.zero();
}