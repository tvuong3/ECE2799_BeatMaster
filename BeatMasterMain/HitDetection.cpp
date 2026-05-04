#include "HitDetection.h"
#include "Arduino.h"


// how hard you need to hit before it counts as a "hit". filters out background noise
const int hitThreshold = 300; // replace with tested value
// min time btwn hits, prevents one hit from being registered twice
const unsigned long debounce_ms = 80; // replace with tested value
// how long after a hit is detected we keep reading to find the peak force value
const unsigned long peak_window_ms = 20; // replace with tested value

// initialize variables
// unsigned long lastHitTime = 0;
// bool inPeakWindow = false;
// unsigned long peakWindowStart = 0;
// int peakValue = 0;
// Hit latestHit = {0, 0};
// static bool hitReady = false;



void hitDetectionInit() {
  pinMode(PIEZO_PIN, INPUT);
}

float getVoltage(int inputPin) {
  int voltage = analogRead(inputPin);
  float adjustedVoltage = voltage * (3.3 / 4096); // Converting the 12-bit ADC output to a 0V - 3.3V range
  return adjustedVoltage;
}

Hit waitForHit(int inputPin, float thresholdVoltage, unsigned long timeToWaitTill) {
  static unsigned long lastHitTime = 0;
  Hit recordedHit;
  float recordedVoltage = 0;
  bool passedThreshold = false;
  unsigned long timeStamp = 0;
  // Waiting until the next hit is recorded or the time is passed
  while((!passedThreshold) && millis() < timeToWaitTill) {
    recordedVoltage = getVoltage(inputPin);
    if(recordedVoltage > thresholdVoltage) {
      if (millis() - lastHitTime > debounce_ms){
      timeStamp = millis();
      lastHitTime = timeStamp; 
      passedThreshold = true;
      break;
    }
   }
       delay(0);
  }
  recordedHit.voltage = recordedVoltage;
  recordedHit.timestamp = timeStamp;

  if (millis() > timeToWaitTill) {
    // Hit never actually came in time
    // Set timestamp to 0 as an indicator there was no hit
    recordedHit.timestamp = 0; 
  } 
  return recordedHit;
}

// void readSensor() {
//   int sensorValue = analogRead(PIEZO_PIN);
//   unsigned long now = millis();

//   if (inPeakWindow) {
//     if (sensorValue > peakValue) {
//       peakValue = sensorValue;
//     }

//     if (now - peakWindowStart >= peak_window_ms) {
//       inPeakWindow = false;
//       lastHitTime = peakWindowStart;
//       latestHit.timestamp = peakWindowStart;
//       latestHit.force = peakValue;
//       hitReady = true;
//     }
//     return;
//   }

//   if (sensorValue > hitThreshold && (now - lastHitTime > debounce_ms)) {
//     inPeakWindow = true;
//     peakWindowStart = now;
//     peakValue = sensorValue;
//     hitReady = false;
//   }
// }