#include "tasks\Nema17.h"

Nema17::Nema17(int pinDir, int pinStep) {
    this->pinDir = pinDir;
    this->pinStep = pinStep;
}

void Nema17::setup() {
    pinMode(this->pinDir, OUTPUT);
    pinMode(this->pinStep, OUTPUT);
};

void Nema17::fullRev(int loops, int forward) {
  if (forward)
    digitalWrite(this->pinDir, HIGH);
  else
    digitalWrite(this->pinDir, LOW);
   
  for(int i = 0; i<(this->steps_per_rev*loops); i++)
  {
    digitalWrite(this->pinStep, HIGH);
    delayMicroseconds(2000);
    digitalWrite(this->pinStep, LOW);
    delayMicroseconds(2000);
  }
};