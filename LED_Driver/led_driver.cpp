#include "led_driver.h"

LED_Driver::LED_Driver(int sdiPinIn, int clkPinIn, int latchPinIn, int oePinIn = -1) {
  sdiPin = sdiPinIn;
  clkPin = clkPinIn;
  latchPin = latchPinIn;
  oePin = oePinIn;
  pinMode(sdiPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  digitalWrite(latchPin, LOW);
  if(oePin != -1) {
    pinMode(oePin, OUTPUT);
    digitalWrite(oePin, HIGH);
  }
}

void LED_Driver::writePattern(uint8_t pattern) {
  digitalWrite(latchPin, LOW);
  Serial.print("Latch Set Low\n");
  uint8_t bitVal = 0;
  for (int i = 8; i > 0, i--;){
    digitalWrite(clkPin, LOW);

    bitVal = (pattern >> i) & 1;
    digitalWrite(sdiPin, bitVal);
    digitalWrite(clkPin, HIGH);
  }
  digitalWrite(latchPin, HIGH);
  Serial.print("Latch Set High\n");
}

void LED_Driver::lightLED(int index) {
  uint8_t pattern = (1 << (index));
  writePattern(pattern);
}

void LED_Driver::setBrightness(int brightness) {
  // Since Output Enable is active low, need to write the opposite duty cycle
  analogWrite(oePin, 255 - (brightness * 255 / 100));
}

void LED_Driver::testHigh(int pin) {
  digitalWrite(sdiPin, HIGH);
  digitalWrite(pin, HIGH);
}