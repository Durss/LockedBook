#include <EEPROM.h>

const int piezo = A0;

void setup() {
  Serial.begin(9600);

  pinMode(piezo, OUTPUT);
  tone(piezo, 800);
  delay(50);
  noTone(piezo);
  delay(50);
  tone(piezo, 800);
  delay(100);
  
  setupPiano();
  setupKnock();
  setupStepper();
}

void loop() {
  loopPiano();
  loopKnock();
  loopStepper();
}
