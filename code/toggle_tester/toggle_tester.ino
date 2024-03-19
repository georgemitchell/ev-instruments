// Define the pin connections for the CAN 2515 module
#define TOGGLE_PIN 8

#define BOUNCE_THRESHOLD 10

volatile bool toggled = false;

unsigned long timestamp;
unsigned long lastToggle = 0;

int switchState = -1;
int currentSwitchState;

bool positionOn = false;

void setup() {
  // put your setup code here, to run once:
  while (!Serial);
  Serial.begin(9600);

  pinMode(TOGGLE_PIN, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(TOGGLE_PIN), SwitchToggled, CHANGE);

}

void loop() {
  currentSwitchState = digitalRead(TOGGLE_PIN);
  if (currentSwitchState != switchState) {
    switchState = currentSwitchState;
    UpdateState();
  }
}

void UpdateState()
{
  if (switchState == HIGH) {
    positionOn = true;
    Serial.println("Toggle is in ON position");
  } else {
    positionOn = false;
    Serial.println("Toggle is in OFF position");
  }
}

void SwitchToggled() {
  Serial.println("toggled!");
}
