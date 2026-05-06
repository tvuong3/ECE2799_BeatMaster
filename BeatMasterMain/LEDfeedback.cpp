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
  strip.setBrightness(50);
  for (int i = 0; i < 8; i++) {
    lightLEDs(i);
    delay(400);
  }
  clearFeedback();
  delay(500);
}



// EXTERNAL FUNCTIONS

void setupLedStrip(){
  startUpTest();
};


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




