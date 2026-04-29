#ifndef HIT_DETECTION_H
#define HIT_DETECTION_H

#define PIEZO_PIN 99 // replace later

struct Hit {
  unsigned long timestamp;
  float voltage;
};

// variables
Hit latestHit;

// functions
void hitDetectionInit();
void readSensor();
bool hitAvailable();

#endif

/////////// matt's code

// #define PIEZO_PIN A0

float getVoltage(int inputPin);

Hit waitForHit(int inputPin, float thresholdVoltage, unsigned long timeToWaitTill);

// void setup() {
//   // put your setup code here, to run once:
//
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   float thresholdValue = 0.5;
//   float outputVoltage = waitForHit(PIEZO_PIN, thresholdValue);
//   Serial.print("Hit Detected!\n");
//   Serial.print("Voltage: ");
//   Serial.print(outputVoltage);
//   Serial.print("\n");
//   delay(37.5);  // Delaying by the half time between sixteenth notes at 200bpm
// }