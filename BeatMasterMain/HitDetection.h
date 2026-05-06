#ifndef HIT_DETECTION_H
#define HIT_DETECTION_H

#define PIEZO_PIN 36 // replace later

struct Hit {
  unsigned long timestamp;
  float voltage;
};

// variables
//extern Hit latestHit;

// functions
void hitDetectionInit();
Hit waitForHit(int inputPin, float thresholdVoltage, unsigned long timeToWaitTill);

//void readSensor();
//bool hitAvailable();

#endif


// #define PIEZO_PIN A0

float getVoltage(int inputPin);

Hit waitForHit(int inputPin, float thresholdVoltage, unsigned long timeToWaitTill);

