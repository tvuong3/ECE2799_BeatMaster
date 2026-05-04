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
  uint8_t bitVal = 0;
  for (int i = 7; i >= 0; i--){
    digitalWrite(clkPin, LOW);

    bitVal = (pattern >> i) & 1;
    digitalWrite(sdiPin, bitVal);
    digitalWrite(clkPin, HIGH);
  }
  digitalWrite(latchPin, HIGH);
}

void LED_Driver::lightLED(int index) {
  uint8_t pattern = (1 << (index));
  writePattern(pattern);
}

void LED_Driver::setBrightness(int brightness) {
  // Since Output Enable is active low, need to write the opposite duty cycle
  ledcAttach(oePin, 1000, 8);
  ledcWrite(oePin, 255 - (brightness * 255 / 100));
  // analogWrite(oePin, 255 - (brightness * 255 / 100));
}

void LED_Driver::testHigh(int pin) {
  digitalWrite(sdiPin, HIGH);
  digitalWrite(pin, HIGH);
}

void LED_Driver::startupSequence() {
  setBrightness(50);
  lightLED(0);
  delay(200);
  lightLED(1);
  delay(200);
  lightLED(2);
  delay(200);
  lightLED(3);
  delay(200);
  lightLED(4);
  delay(200);
  lightLED(5);
  delay(200);
  lightLED(6);
  delay(200);
  lightLED(7);
  delay(200);
  // Turning off LEDs
  // writePattern(0b00000000);
}