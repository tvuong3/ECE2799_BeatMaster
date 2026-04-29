#include "LEDFeedback.h"
#include "Arduino.h"

unsigned long lastFeedbackTime = 0;

// create instances of LED_Driver
LED_Driver strip(STRIP_DATA_PIN, STRIP_CLOCK_PIN, STRIP_LATCH_PIN, STRIP_OE_PIN);
LED_Driver ring(RING_DATA_PIN, RING_CLOCK_PIN, RING_LATCH_PIN, RING_OE_PIN);

// internal function declarations
int getAccuracy(long error);
void lightLEDs(int index);
void startUpTest();


// INTERNAL FUNCTIONS

int getAccuracy(long error) {
  if (error < -120) return 0; // left red
  else if (error < -60) return 1; // left yellow 2
  else if (error < -30) return 2; // left yellow 1
  else if (error <= 30) return 3; // green
  else if (error <= 60) return 4; // right yellow 1
  else if (error <= 120) return 5; // right yellow 2
  else return 6; // right red
}

void lightLEDs(int index) {
  // strip has 7 LEDs, index 0-6
  // each LED is one bit in the pattern
  strip.writePattern(1 << index);
}


void startUpTest() {
  for (int i = 0; i < 7; i++) {
    lightLEDs(i);
    delay(400);
  }
  clearFeedback();
  delay(500);
}



// EXTERNAL FUNCTIONS

void setupLedStrip() {
  // pinMode(DATA_PIN, OUTPUT);
  // pinMode(CLOCK_PIN, OUTPUT);
  // pinMode(LATCH_PIN, OUTPUT);
  // pinMode(OE_PIN, OUTPUT);
  // digitalWrite(OE_PIN, LOW);
  startUpTest();
}

void setupRing() {
  // pinMode(RING_LATCH_PIN, OUTPUT);
  // pinMode(RING_OE_PIN, OUTPUT);
  // SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  // digitalWrite(RING_LATCH_PIN, LOW);
  // SPI.transfer(0xFF);
  // digitalWrite(RING_LATCH_PIN, HIGH);
  // delayMicroseconds(1);
  // digitalWrite(RING_LATCH_PIN, LOW);
  // SPI.endTransaction();
  // ledcAttach(RING_OE_PIN, 1000, 8);
  // ledcWrite(1, 255);

  // PWM on OE pin controls brightness
  ledcAttach(RING_OE_PIN, 1000, 8);
  ledcWrite(RING_OE_PIN, 255); // off initially
  // enable all 16 LEDs via shift register
  // two chained chips so write twice
  ring.writePattern(0xFF);
  ring.writePattern(0xFF);

}


void showAccuracy(long error, HitFeedback type) {
  if (type == FEEDBACK_EXTRA) {
    lightLEDs(0); // left red = extra hit
  } else if (type == FEEDBACK_MISSED) {
    lightLEDs(6); // right red = missed hit
  } else {
  lightLEDs(getAccuracy(error));
  }
  lastFeedbackTime = millis();
}


void showIntensity(int peakValue) {
  int brightness = map(peakValue, 0, 3.3, 100, 0);
  // maybe 255 instead of 100? 
  //ledcWrite(RING_OE_PIN, brightness);
  ring.setBrightness(brightness);
}


void clearFeedback() {
  strip.writePattern(0b00000000);
  ledcWrite(RING_OE_PIN, 255);
}


// void writeAccuracyStrip(byte pattern) {
//   digitalWrite(LATCH_PIN, LOW);
//   for (int i = 7; i >= 0; i--) {
//     digitalWrite(CLOCK_PIN, LOW);
//     int bitVal = (pattern >> i) & 1;
//     digitalWrite(DATA_PIN, bitVal);
//     digitalWrite(CLOCK_PIN, HIGH);
//   }
//   digitalWrite(LATCH_PIN, HIGH);
// }



