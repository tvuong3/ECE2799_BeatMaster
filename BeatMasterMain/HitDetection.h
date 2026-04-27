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




/////////// matt's code

// #define PIEZO_PIN A0

// void setup() {
//   // put your setup code here, to run once:
//   pinMode(PIEZO_PIN, INPUT);
// }

// float waitForHit(int inputPin, float thresholdVoltage) {
//   int voltage = 0;
//   float adjustedVoltage = 0;
//   bool passedThreshold = false;
//   while(!passedThreshold) {
//     voltage = analogRead(inputPin);
//     adjustedVoltage = voltage * (3.3 / 4096);   // Converting the 12-bit ADC output to a 0V - 3.3V range
//     if(adjustedVoltage > thresholdVoltage) {
//       passedThreshold = true;
//     }
//   }
//   return adjustedVoltage;
// }

// void loop() {
//   // put your main code here, to run repeatedly: 
//   float thresholdValue = 0.5;
//   float outputVoltage = waitForHit(PIEZO_PIN, thresholdValue);
//   Serial.print("Hit Detected!\n");
//   Serial.print("Voltage: ");
//   Serial.print(outputVoltage);
//   Serial.print("\n");
//   delay(37.5);  // Delaying by the half time between sixteenth notes at 200bpm
// }