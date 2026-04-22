#ifndef LED_FEEDBACK_H
#define LED_FEEDBACK_H

// strip pins
#define DATA_PIN 2    // replace later
#define CLOCK_PIN 3   // replace later
#define LATCH_PIN 4   // replace later
#define OE_PIN 5      // replace later

// ring pins
#define RING_LATCH_PIN 6  // replace later
#define RING_OE_PIN 7     // replace later

extern unsigned long lastFeedbackTime;

void setupLedStrip();
void setupRing();
void showAccuracy(long error);
void showIntensity(int peakValue);
void clearFeedback();

#endif