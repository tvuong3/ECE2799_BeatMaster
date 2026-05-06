#ifndef LED_FEEDBACK_H
#define LED_FEEDBACK_H
#include "led_driver.h"


// strip pins
#define STRIP_DATA_PIN 2 
#define STRIP_CLOCK_PIN 0
#define STRIP_LATCH_PIN 4 
#define STRIP_OE_PIN 16 

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