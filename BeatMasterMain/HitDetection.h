#ifndef HIT_DETECTION_H
#define HIT_DETECTION_H

#define PIEZO_PIN 99  // replace later

struct Hit {
  unsigned long timestamp;
  int force;
};

// variables
extern unsigned long lastHitTime;
extern bool inPeakWindow;
extern unsigned long peakWindowStart;
extern int peakValue;
extern Hit latestHit;

// functions
void hitDetectionInit();
void readSensor();
bool hitAvailable();

#endif