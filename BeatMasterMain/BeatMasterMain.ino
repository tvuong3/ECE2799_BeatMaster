#include "Menu.h"
#include "SDReader.h"
#include "Controls.h"
#include "HitDetection.h"
#include "LEDfeedback.h"
#include "Scoring.h"
#include "metronome.h"
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include <SPI.h>

// pin assignments
#define PIEZO_PIN 14
#define LED_STRIP_PIN 99
#define LED_RING_PIN 99
#define SPEAKER_PIN  99

//led strip driver
//#define DATA_PIN 2
//#define CLOCK_PIN 3
//#define LATCH_PIN 4
//#define OE_PIN 5

//led ring 
//#define RING_LATCH_PIN 
//#define RING_OE_PIN

Metronome metro(SPEAKER_PIN);

// menu state machine
enum State {
  STATE_POWER_ON, 
  STATE_MAIN_MENU, 
  STATE_FREE_PLAY, 
  STATE_RUDIMENT_SELECT,
  STATE_TEMPO_SET,
  STATE_RUDIMENT_PREVIEW,
  STATE_RUDIMENT_LEADIN, 
  STATE_RUDIMENT_PRACTICE, 
  STATE_RESULTS,
  STATE_SOUND_LIBRARY
};

State currentState = STATE_POWER_ON;
String selectedRudiment = "";
int score = 0;
unsigned long expectedBeatTime = 0;
int leadInCount = 0;

void setup() {
  Serial.begin(115200);
  // while(!Serial);
  // delay(5000);
  Serial.println("Setup started");
  // LCD
  Config_Init();
    Serial.println("config started");

  LCD_Init();
    Serial.println("init started");

  //LCD_Clear(BLACK);
    Serial.println("clear started");

  LCD_Clear(0xF800);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, BLACK);
  Paint_Clear(BLACK);
  // test
  Paint_DrawString_EN(10, 10, "BeatMaster+", &Font24, WHITE, WHITE);
  Paint_DrawString_EN(0, 32, "LCD working!", &Font16, WHITE, WHITE);


  // // LCD
  // Config_Init();
  // LCD_Init();
  // LCD_Clear(BLACK);
  // Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, BLACK);
  // Paint_Clear(BLACK);

  // // modules
  // sdInit();
  // menuInit();
  // controlsInit();
  // hitDetectionInit();
  // setupLedStrip();
  // setupRing();

  // metronome
  metro.begin();
  metro.setMeasure(4);

}


void loop() {
  static bool screenInitialized = false;
  if(!screenInitialized) {
    Serial.print("Initializing screen in Loop\n");
    Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, BLUE);
    digitalWrite(DEV_BL_PIN, HIGH);
    delay(1000);
    screenInitialized = true;
  }
  Paint_Clear(GREEN);
  // controlsUpdate();
  Serial.print("We Loopin\n");



  switch (currentState) {

    case STATE_POWER_ON:
    Serial.print("Current State: Power On\n");
     Paint_Clear(BLACK);
     Paint_DrawString_EN(0, 100, "BeatMaster+", &Font24, WHITE, BLACK);
     delay(2000);
     currentState = STATE_MAIN_MENU;
     break;

     case STATE_MAIN_MENU: {
      Serial.print("Current State: Menu\n");
      Paint_Clear(BLACK);
      Paint_DrawString_EN(0, 0, "Select Mode:", &Font16, GREEN, BLACK);
      Paint_DrawString_EN(0, 32, currentOption == FREE_PLAY ? "> Free Play" : "  Free Play", &Font16, GREEN, BLACK);
      Paint_DrawString_EN(0, 48, currentOption == RUDIMENTS ? "> Rudiments" : "  Rudiments", &Font16, WHITE, WHITE);
      Paint_DrawString_EN(0, 64, currentOption == SOUND_LIBRARY ? "> Sound Library" : "  Sound Library", &Font16, WHITE, WHITE);
      Paint_DrawCircle(0, 100, 10)
      
      delay(3000);
      // MenuOption selected = menuUpdate();
      // if (selected == FREE_PLAY) currentState = STATE_FREE_PLAY;
      // if (selected == RUDIMENTS) currentState = STATE_RUDIMENT_SELECT;
      // if (selected == SOUND_LIBRARY) currentState = STATE_SOUND_LIBRARY;
      // TESTING PURPOSES ONLY
      currentState = STATE_FREE_PLAY;
      break;
    }

    case STATE_FREE_PLAY:
      Serial.print("Current State: Freeplay\n");
      Paint_Clear(LIGHTBLUE);
      Paint_DrawString_EN(0, 0, "Free Play", &Font16, LIGHTBLUE, BLACK);
      Paint_DrawString_EN(0, 32, metronomeOn ? "Metronome: ON" : "Metronome: OFF", &Font16, LIGHTBLUE, BLACK);
      Paint_DrawString_EN(0, 48, "Click: toggle metro", &Font16, LIGHTBLUE, BLACK);
      Paint_DrawString_EN(0, 64, "Left: exit", &Font16, LIGHTBLUE, BLACK);
      delay(1000);
      metro.setBeatsPerMinute(currentBPM);
      if (metronomeOn) metro.check();
      readSensor();
      if (hitAvailable()) {
        // just show intensity, no scoring in free play
        showIntensity(peakValue);
      }
      if (!freePlayUpdate()) {
        metro.stop();
        metronomeOn = false;
        clearFeedback();
        currentState = STATE_MAIN_MENU;
      }
      break;

    case STATE_RUDIMENT_SELECT: {
      Serial.print("Current State: Rudiment Select\n");
      Paint_Clear(BLACK);
      Paint_DrawString_EN(0, 0, "Select Rudiment:", &Font16, WHITE, BLACK);
      int count = getFileCount("/rudiments");
      for (int i = 0; i < count; i++) {
        String name = getFileName("/rudiments", i);
        String line = (i == rudimentIndex ? "> " : "  ") + name;
        Paint_DrawString_EN(0, 16 + i * 16, line.c_str(), &Font16, WHITE, BLACK);
      }
      String result = RudimentMenuUpdate();
      if (result != "") {
        selectedRudiment = result;
        currentState = STATE_TEMPO_SET;
      }
      break;
    }

    case STATE_TEMPO_SET: {
      Paint_Clear(BLACK);
      Paint_DrawString_EN(0, 0, "Set Tempo:", &Font16, WHITE, BLACK);
      char bpmStr[10];
      sprintf(bpmStr, "%d BPM", currentBPM);
      Paint_DrawString_EN(0, 32, bpmStr, &Font24, WHITE, BLACK);
      Paint_DrawString_EN(0, 100, "Click to confirm", &Font16, WHITE, BLACK);
      Paint_DrawString_EN(0, 116, "Left to go back", &Font16, WHITE, BLACK);
      TempoResult result = tempoUpdate();
      if (result == TEMPO_CONFIRMED) currentState = STATE_RUDIMENT_PREVIEW;
      if (result == TEMPO_BACK) currentState = STATE_RUDIMENT_SELECT;
      break;
    }

    case STATE_RUDIMENT_PREVIEW:
      Paint_Clear(BLACK);
      Paint_DrawString_EN(0, 0, "Listen:", &Font16, WHITE, BLACK);
      Paint_DrawString_EN(0, 16, selectedRudiment.c_str(), &Font16, WHITE, BLACK);
      // TODO: audio partner plays the file here
      // when audio done → automatically move on
      currentState = STATE_RUDIMENT_LEADIN;
      break;

    case STATE_RUDIMENT_LEADIN: {
      Paint_Clear(BLACK);
      Paint_DrawString_EN(0, 0, "Get Ready...", &Font16, WHITE, BLACK);
      metro.setBeatsPerMinute(lockedBPM);
      metro.start();
      // count 4 beats then start
      if (metro.beat()) {
        leadInCount++;
      }
      if (leadInCount >= 4) {
        leadInCount = 0;
        scoringInit();
        exerciseStartTime = millis();
        exerciseRun = true;
        currentState = STATE_RUDIMENT_PRACTICE;
      }
      break;
    }

    case STATE_RUDIMENT_PRACTICE: {
      // countdown display
      unsigned long elapsed = millis() - exerciseStartTime;
      unsigned long remaining = (EXERCISE_LENGTH - elapsed) / 1000;
      char timerStr[10];
      sprintf(timerStr, "%lu sec", remaining);
      Paint_Clear(BLACK);
      Paint_DrawString_EN(0, 0, timerStr, &Font24, WHITE, BLACK);
      Paint_DrawString_EN(0, 60, "Click: STOP", &Font16, WHITE, BLACK);

      metro.check();

      // track expected beat time
      if (metro.beat()) {
        expectedBeatTime = millis();
      }

      readSensor();
      if (hitAvailable()) {
        processHit(latestHit.timestamp, latestHit.force, expectedBeatTime);
        long error = (long)latestHit.timestamp - (long)expectedBeatTime;
        showAccuracy(error);
        showIntensity(latestHit.force);
      }

      // clear feedback after duration
      if (lastFeedbackTime > 0 && millis() - lastFeedbackTime >= FEEDBACK_DURATION_MS) {
        clearFeedback();
        lastFeedbackTime = 0;
      }

      runExerciseTimer();

      // stop button
      if (digitalRead(JOY_SW) == LOW) {
        metro.stop();
        clearFeedback();
        scoringInit();
        currentState = STATE_RUDIMENT_SELECT;
      }

      if (exerciseFinished()) {
        metro.stop();
        clearFeedback();
        score = getScore();
        currentState = STATE_RESULTS;
      }
      break;
    }

    case STATE_RESULTS: {
      Paint_Clear(BLACK);
      char scoreStr[20];
      sprintf(scoreStr, "Score: %d%%", score);
      Paint_DrawString_EN(0, 0, scoreStr, &Font24, WHITE, BLACK);
      Paint_DrawString_EN(0, 60, currentOption == 0 ? "> Try Again" : "  Try Again", &Font16, WHITE, BLACK);
      Paint_DrawString_EN(0, 76, currentOption == 1 ? "> Exit" : "  Exit", &Font16, WHITE, BLACK);
      MenuOption selected = menuUpdate();
      if (selected != NONE) {
        if (currentOption == 0) currentState = STATE_TEMPO_SET;
        if (currentOption == 1) currentState = STATE_RUDIMENT_SELECT;
      }
      break;
    }

    case STATE_SOUND_LIBRARY: {
      Paint_Clear(BLACK);
      Paint_DrawString_EN(0, 0, "Sound Library:", &Font16, WHITE, BLACK);
      int count = getFileCount("/soundlibrary");
      for (int i = 0; i < count; i++) {
        String name = getFileName("/soundlibrary", i);
        String line = (i == activeSound ? "* " : "  ") + name;
        Paint_DrawString_EN(0, 16 + i * 16, line.c_str(), &Font16, WHITE, BLACK);
      }
      String result = SoundMenuUpdate();
      if (result != "") currentState = STATE_MAIN_MENU;
      break;
    }
  }
}

// const int hitThreshold = ??;
// const unsigned long debounce_ms = ??;
// const unsigned long peak_window_ms = ??;

// unsigned long lastHitTime = 0;
// bool inPeakWindow = false;
// unsigned long peakWindowStart = 0;
// int peakValue = 0;

// unsigned long exerciseStartTime = 0;
// unsigned long expectedBeatTime = 0;
// const unsigned long exercise_Length = 30000;
// bool exerciseRun = false;

// int totalHits = 0;
// int correctHits = 0;

// unsigned long lastFeedbackTime = 0;

// //led strip functions
// void writeAccuracyStrip(byte pattern){
//   digitalWrite(LATCH_PIN, LOW);

//   for (int i = 7; i>=0; i--){
//     digitalWrite(CLOCK_PIN, LOW);

//     int bitVal = (pattern >> i) & 1;
//     digitalWrite(DATA_PIN, bitVal);
//     digitalWrite(CLOCK_PIN, HIGH);
//   }
//   digitalWrite(LATCH_PIN, HIGH);
// }

// void lightLED(int index){
//   byte pattern = (1 << (6 - index));
//   writeAccuracyStrip(pattern);
// }

// int getAccuracy(long error){
//   if (error < -120) return 0;
//   else if (error < -60) return 1;
//   else if (error < -30) return 2;
//   else if (error <= 30) return 3;
//   else if (error <= 60) return 4;
//   else if (error <= 120) return 5;
//   else return 6;
// }

// void showAccuracy(long error){
//   int index = getAccuracy(error);
//   lightLED(index);
//   lastFeedbackTime = millis();

//   Serial.print("Error: ");
//   Serial.print(error);
//   Serial.print(" ms -> LED");
//   Serial.println(index);
// }

// void clearFeedback (){
//   writeAccuracyStrip(0b00000000);
//   clearRing();
// }

// void startUpTest(){
//   for (int i = 0; i<7; i++){
//     lightLED(i);
//     delay(400);
//   }
//   clearFeedback();
//   delay(500);
// }

// void setupLedStrip() {
//   pinMode(DATA_PIN, OUTPUT);
//   pinMode(CLOCK_PIN, OUTPUT);
//   pinMode(LATCH_PIN, OUTPUT);
//   pinMode(OE_PIN, OUTPUT);
//   digitalWrite(OE_PIN, LOW);

//   startUpTest();
// }

// // LED Ring Intensity
// void setupRing(){
//   pinMode(RING_LATCH_PIN, OUTPUT);
//   pinMode(RING_OE_PIN, OUTPUT);

//   SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
//   digitalWrite(RING_LATCH_PIN, LOW);
//   SPI.transfer(0xFF);
//   digitalWrite(RING_LATCH_PIN, HIGH);
//   delayMicroseconds(1);
//   digitalWrite(RING_LATCH_PIN, LOW);
//   SPI.endTransaction();

//   ledcSetup(1, 1000, 8);
//   ledcAttachPin(RING_OE_PIN, 1);
//   ledcWrite(1,255);
// }

// void showIntensity(int peakValue){
//   int brightness = map(peakValue, 0, 4095, 255, 0);
//   // OE active low, hard hit =low OE = bright
//   ledcWrite(1, brightness);
// }

// void clearRing(){
//   ledcWrite(1, 255); //OE high - all LEDs off
// }

// // hit processing
// void processHit(unsigned long hitTime, int intensity) {
//   totalHits++;
//   long error = (long)hitTime - (long)expectedBeatTime;
//   long absError = abs(error);
//   if (abs(error) <= 30){
//     correctHits++;

//     showAccuracy(error);
//     showIntensity(intensity);
// }

// //sensor reading
// void readSensor() {
//   int sensorValue = analogRead(PIEZO_PIN);
//   unsigned long now = millis();
  
//   if (inPeakWindow){
//     if (sensorValue > peakValue) peakValue = sensorValue;

//     if(now - peakWindowStart >= peak_window_ms){
//       inPeakWindow = false;
//       lastHitTime = peakWindowStart;
//       processHit(peakWindowStart, peakValue);
//     }
//     return;
// }
  
//   if (sensorValue > hitThreshold && (now - lastHitTime > debounce_ms)){
//     inPeakWindow = true;
//     peakWindowStart = now;
//     peakValue = sensorValue;
//   }
// }

// void runMetronome(){
//   if (metro.beat()){
//     expectedBeatTime = millis();
//   }
// }

// void runExerciseTimer() {
//   if (millis() - exerciseStartTime >= exercise_Length){
//     exerciseRun = false;
//     clearFeedback();

//     int score = (totalHits > 0) ? (correctHits * 100 / totalHits) : 0;

//     Serial.print("Hits: "); Serial.println(totalHits);
//     Serial.print("Correct: "); Serial.println(correctHits);
//     Serial.print("Score: "); Serial.print(score); Serial.println("%");
//   }
// }

// void setup() {
// // put your setup code here, to run once:
//   Serial.begin(115200);

//   setupLedStrip();
//   setupRing();

//   pinMode(LED_STRIP_PIN, OUTPUT);
//   pinMode(LED_RING_PIN, OUTPUT);
//   pinMode(SPEAKER_PIN, OUTPUT);
//   pinMode(PIEZO_PIN, INPUT);

//   metro.begin();
//   metro.setBeatsPerMinute(120);
//   metro.setMeasure(4);
//   metro.start();

//   exerciseStartTime = millis();
//   exerciseRun = true;
// }

// void loop() {
// // put your main code here, to run repeatedly:
//   if (!exerciseRun) return;
//   metro.check();
//   readSensor();
//   runMetronome();
//   runExerciseTimer();

//   if (lastFeedbackTime > 0 && millis () - lastFeedbackTime >= FEEDBACK_DURATION_MS){
//     clearFeedback();
//     lastFeedbackTime = 0;
//   }
// }

