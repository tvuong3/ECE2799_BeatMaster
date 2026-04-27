#include "LEDFeedback.h"
#include "Arduino.h"
#include <SPI.h>

unsigned long lastFeedbackTime = 0;

void writeAccuracyStrip(byte pattern) {
  digitalWrite(LATCH_PIN, LOW);
  for (int i = 7; i >= 0; i--) {
    digitalWrite(CLOCK_PIN, LOW);
    int bitVal = (pattern >> i) & 1;
    digitalWrite(DATA_PIN, bitVal);
    digitalWrite(CLOCK_PIN, HIGH);
  }
  digitalWrite(LATCH_PIN, HIGH);
}

int getAccuracy(long error) {
  if (error < -120) return 0; // left red
  else if (error < -60) return 1; // left yellow 2
  else if (error < -30) return 2; // left yellow 1
  else if (error <= 30) return 3; // green
  else if (error <= 60) return 4; // right yellow 1
  else if (error <= 120) return 5; // right yellow 2
  else return 6; // right red
}

void lightLED(int index) {
  byte pattern = (1 << (6 - index));
  writeAccuracyStrip(pattern);
}

void showAccuracy(long error, HitFeedback type) {
  if (type == FEEDBACK_EXTRA) {
    lightLED(0); // left red = extra hit
  } else if (type == FEEDBACK_MISSED) {
    lightLED(6); // right red = missed hit
  } else {
  int index = getAccuracy(error);
  lightLED(index);
  }
  lastFeedbackTime = millis();
}

void showIntensity(int peakValue) {
  int brightness = map(peakValue, 0, 4095, 255, 0);
  ledcWrite(RING_OE_PIN, brightness);
}

void clearFeedback() {
  writeAccuracyStrip(0b00000000);
  ledcWrite(1, 255);
}

void startUpTest() {
  for (int i = 0; i < 7; i++) {
    lightLED(i);
    delay(400);
  }
  clearFeedback();
  delay(500);
}

void setupLedStrip() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, LOW);
  startUpTest();
}

void setupRing() {
  pinMode(RING_LATCH_PIN, OUTPUT);
  pinMode(RING_OE_PIN, OUTPUT);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(RING_LATCH_PIN, LOW);
  SPI.transfer(0xFF);
  digitalWrite(RING_LATCH_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(RING_LATCH_PIN, LOW);
  SPI.endTransaction();
  ledcAttach(RING_OE_PIN, 1000, 8);
  ledcWrite(1, 255);
}