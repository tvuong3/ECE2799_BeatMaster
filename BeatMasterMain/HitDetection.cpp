#include "HitDetection.h"
#include "Arduino.h"

// how hard you need to hit before it counts as a "hit". filters out background noise
const int hitThreshold = 100;      // replace with tested value
// min time btwn hits, prevents one hit from being registered twice
const unsigned long debounce_ms = 50;   // replace with tested value
// how long after a hit is detected we keep reading to find the peak force value
const unsigned long peak_window_ms = 20; // replace with tested value

// initialize variables
unsigned long lastHitTime = 0;
bool inPeakWindow = false;
unsigned long peakWindowStart = 0;
int peakValue = 0;
Hit latestHit = {0, 0};
static bool hitReady = false;


void hitDetectionInit() {
  pinMode(PIEZO_PIN, INPUT);
}

bool hitAvailable() {
  return hitReady;
}

void readSensor() {
  int sensorValue = analogRead(PIEZO_PIN);
  unsigned long now = millis();

  if (inPeakWindow) {
    if (sensorValue > peakValue) peakValue = sensorValue;

    if (now - peakWindowStart >= peak_window_ms) {
      inPeakWindow = false;
      lastHitTime = peakWindowStart;
      latestHit.timestamp = peakWindowStart;
      latestHit.force = peakValue;
      hitReady = true;
    }
    return;
  }

  if (sensorValue > hitThreshold && (now - lastHitTime > debounce_ms)) {
    inPeakWindow = true;
    peakWindowStart = now;
    peakValue = sensorValue;
    hitReady = false;
  }
}