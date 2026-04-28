#ifndef LED_FEEDBACK_H
#define LED_FEEDBACK_H
#include "led_driver.h"


// strip pins
#define STRIP_DATA_PIN 2 // replace later
#define STRIP_CLOCK_PIN 3 // replace later
#define STRIP_LATCH_PIN 4 // replace later
#define STRIP_OE_PIN 5 // replace later

// ring pins
#define RING_DATA_PIN 6 // replace later
#define RING_CLOCK_PIN 7 // replace later
#define RING_LATCH_PIN 8 // replace later
#define RING_OE_PIN 9 // replace later

enum HitFeedback {
  FEEDBACK_TIMED,
  FEEDBACK_EXTRA,
  FEEDBACK_MISSED
};

extern unsigned long lastFeedbackTime;

void setupLedStrip();
void setupRing();
void showAccuracy(long error, HitFeedback type = FEEDBACK_TIMED);
void showIntensity(int peakValue);
void clearFeedback();

#endif