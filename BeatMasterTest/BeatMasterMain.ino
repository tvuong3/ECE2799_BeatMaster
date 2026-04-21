#include <metronome.h>
#include <SPI.h>

#define PIEZO_PIN 14
#define LED_STRIP_PIN 
#define LED_RING_PIN 
#define SPEAKER_PIN 

//led strip driver
#define DATA_PIN 2
#define CLOCK_PIN 3
#define LATCH_PIN 4
#define OE_PIN 5

//led ring 
#define RING_LATCH_PIN 
#define RING_OE_PIN

Metronome metro(SPEAKER_PIN);

const int hitThreshold = ??;
const unsigned long debounce_ms = ??;
const unsigned long peak_window_ms = ??;

unsigned long lastHitTime = 0;
bool inPeakWindow = false;
unsigned long peakWindowStart = 0;
int peakValue = 0;

unsigned long exerciseStartTime = 0;
unsigned long expectedBeatTime = 0;
const unsigned long exercise_Length = 30000;
bool exerciseRun = false;

int totalHits = 0;
int correctHits = 0;

unsigned long lastFeedbackTime = 0;

//led strip functions
void writeAccuracyStrip(byte pattern){
  digitalWrite(LATCH_PIN, LOW);

  for (int i = 7; i>=0; i--){
    digitalWrite(CLOCK_PIN, LOW);

    int bitVal = (pattern >> i) & 1;
    digitalWrite(DATA_PIN, bitVal);
    digitalWrite(CLOCK_PIN, HIGH);
  }
  digitalWrite(LATCH_PIN, HIGH);
}

void lightLED(int index){
  byte pattern = (1 << (6 - index));
  writeAccuracyStrip(pattern);
}

int getAccuracy(long error){
  if (error < -120) return 0;
  else if (error < -60) return 1;
  else if (error < -30) return 2;
  else if (error <= 30) return 3;
  else if (error <= 60) return 4;
  else if (error <= 120) return 5;
  else return 6;
}

void showAccuracy(long error){
  int index = getAccuracy(error);
  lightLED(index);
  lastFeedbackTime = millis();

  Serial.print("Error: ");
  Serial.print(error);
  Serial.print(" ms -> LED");
  Serial.println(index);
}

void clearFeedback (){
  writeAccuracyStrip(0b00000000);
  clearRing();
}

void startUpTest(){
  for (int i = 0; i<7; i++){
    lightLED(i);
    delay(400);
  }
  clearFeedback();
  delay(500);
}

void setupLedStrip() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, LOW);

  startUpTest();
}

// LED Ring Intensity
void setupRing(){
  pinMode(RING_LATCH_PIN, OUTPUT);
  pinMode(RING_OE_PIN, OUTPUT);

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(RING_LATCH_PIN, LOW);
  SPI.transfer(0xFF);
  digitalWrite(RING_LATCH_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(RING_LATCH_PIN, LOW);
  SPI.endTransaction();

  ledcSetup(1, 1000, 8);
  ledcAttachPin(RING_OE_PIN, 1);
  ledcWrite(1,255);
}

void showIntensity(int peakValue){
  int brightness = map(peakValue, 0, 4095, 255, 0);
  // OE active low, hard hit =low OE = bright
  ledcWrite(1, brightness);
}

void clearRing(){
  ledcWrite(1, 255); //OE high - all LEDs off
}

// hit processing
void processHit(unsigned long hitTime, int intensity) {
  totalHits++;

  long error = (long)hitTime - (long)expectedBeatTime;
  long absError = abs(error);

  if (abs(error) <= 30){
    correctHits++;

    showAccuracy(error);
    showIntensity(intensity);
}

//sensor reading
void readSensor() {
  int sensorValue = analogRead(PIEZO_PIN);
  unsigned long now = millis();
  
  if (inPeakWindow){
    if (sensorValue > peakValue) peakValue = sensorValue;

    if(now - peakWindowStart >= peak_window_ms){
      inPeakWindow = false;
      lastHitTime = peakWindowStart;
      processHit(peakWindowStart, peakValue);
    }
    return;
}
  
  if (sensorValue > hitThreshold && (now - lastHitTime > debounce_ms)){
    inPeakWindow = true;
    peakWindowStart = now;
    peakValue = sensorValue;
  }
}

void runMetronome(){
  if (metro.beat()){
    expectedBeatTime = millis();
  }
}

void runExerciseTimer() {
  if (millis() - exerciseStartTime >= exercise_Length){
    exerciseRun = false;
    clearFeedback();

    int score = (totalHits > 0) ? (correctHits * 100 / totalHits) : 0;

    Serial.print("Hits: "); Serial.println(totalHits);
    Serial.print("Correct: "); Serial.println(correctHits);
    Serial.print("Score: "); Serial.print(score); Serial.println("%");
  }
}

void setup() {
// put your setup code here, to run once:
  Serial.begin(115200);

  setupLedStrip();
  setupRing();

  pinMode(LED_STRIP_PIN, OUTPUT);
  pinMode(LED_RING_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(PIEZO_PIN, INPUT);

  metro.begin();
  metro.setBeatsPerMinute(120);
  metro.setMeasure(4);
  metro.start();

  exerciseStartTime = millis();
  exerciseRun = true;
}

void loop() {
// put your main code here, to run repeatedly:
  if (!exerciseRun) return;
  metro.check();
  readSensor();
  runMetronome();
  runExerciseTimer();

  if (lastFeedbackTime > 0 && millis () - lastFeedbackTime >= FEEDBACK_DURATION_MS){
    clearFeedback();
    lastFeedbackTime = 0;
  }
}

