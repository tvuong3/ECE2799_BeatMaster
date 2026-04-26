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

// Default SPI Mapping for HSPI, VSPI
// VSPI used for audio files
#define VSPI_COPI_PIN 23  // GPIO 23
#define VSPI_CIPO_PIN 19  // GPIO 19
#define VSPI_CLK_PIN 18   // GPIO 18
#define VSPI_CS_PIN 5     // GPIO 5
//HSPI used for drawing screen
#define HSPI_COPI_PIN 13  // GPIO 13
#define HSPI_CIPO_PIN 12  // GPIO 12
#define HSPI_CLK_PIN 14   // GPIO 14
#define HSPI_CS_PIN 15    // GPIO 15

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
String selectedSound = "";
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

  //LCD_Clear(0xF800);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_90, BLACK);
  //Paint_Clear(BLACK);
  // test
  //Paint_DrawString_EN(10, 10, "BeatMaster+", &Font24, WHITE, WHITE);
  //Paint_DrawString_EN(0, 32, "LCD working!", &Font16, WHITE, WHITE);


  // // LCD
  // Config_Init();
  // LCD_Init();
  // LCD_Clear(BLACK);
  // Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, BLACK);
  // Paint_Clear(BLACK);

  // // modules
  SPIClass* vspi = sdInit(VSPI_CLK_PIN, VSPI_CIPO_PIN, VSPI_COPI_PIN, VSPI_CS_PIN);
  menuInit();
  // controlsInit();
  // hitDetectionInit();
  // setupLedStrip();
  // setupRing();

  // metronome
  metro.begin();
  metro.setMeasure(4);

}

State lastState = STATE_POWER_ON;
MenuOption lastOption = FREE_PLAY;
bool lastMetronomeOn = false;
int lastRudimentIndex = 0;
int lastSoundIndex = 0;

void loop() {
  // static bool screenInitialized = false;
  // if(!screenInitialized) {
  //   Serial.print("Initializing screen in Loop\n");
  //   Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, BLUE);
  //   digitalWrite(DEV_BL_PIN, HIGH);
  //   delay(1000);
  //   screenInitialized = true;
  // }
  // Paint_Clear(GREEN);
  // // controlsUpdate();
  // Serial.print("We Loopin\n");
bool stateChanged = (currentState != lastState);

bool needsRedraw = (currentState != lastState) 
                || (currentOption != lastOption) 
                || (metronomeOn != lastMetronomeOn)
                || (rudimentIndex != lastRudimentIndex)
                || (soundIndex != lastSoundIndex);
lastState = currentState;
lastOption = currentOption;
lastMetronomeOn = metronomeOn;
lastRudimentIndex = rudimentIndex;
lastSoundIndex = soundIndex;

if (stateChanged){
  LCD_Clear(0xF800);
}


  switch (currentState) {

    case STATE_POWER_ON:
    Serial.print("Current State: Power On\n");
    
        LCD_Clear(0xF800); // red
        Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0xF800);
     Paint_DrawString_EN(0, 100, "BeatMaster+", &Font24, WHITE, BLACK);
    
     delay(2000);
     LCD_Clear(0xF800);
     currentState = STATE_MAIN_MENU;
     break;

     case STATE_MAIN_MENU: {
      Serial.print("Current State: Menu\n");
      if (needsRedraw) {
          // LCD_Clear(GREEN); // red
          Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0xF800);
      Paint_DrawString_EN(0, 0, "Select Mode:", &Font16, RED, WHITE);
      Paint_DrawString_EN(0, 32, currentOption == FREE_PLAY ? "> Free Play" : "  Free Play", &Font16, RED, WHITE);
      Paint_DrawString_EN(0, 48, currentOption == RUDIMENTS ? "> Rudiments" : "  Rudiments", &Font16, RED, WHITE);
      Paint_DrawString_EN(0, 64, currentOption == SOUND_LIBRARY ? "> Sound Library" : "  Sound Library", &Font16, RED, WHITE);
      }
    
      MenuOption selected = menuUpdate();
      if (selected == FREE_PLAY) currentState = STATE_FREE_PLAY;
      if (selected == RUDIMENTS) currentState = STATE_RUDIMENT_SELECT;
      if (selected == SOUND_LIBRARY) currentState = STATE_SOUND_LIBRARY;

      break;
    }

    case STATE_FREE_PLAY: 
      Serial.print("Current State: Freeplay\n");
      metro.setBeatsPerMinute(currentBPM);
      if (metronomeOn) metro.check();
      readSensor();
      if (hitAvailable()) {
        showIntensity(peakValue);
        }

      if (!freePlayUpdate()) {
        metro.stop();
        metronomeOn = false;
        clearFeedback();
        LCD_Clear(0xF800);
        currentState = STATE_MAIN_MENU;
      }
      
       if (needsRedraw) {
          LCD_Clear(LIGHTBLUE); // blue
          Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0x001F);
      Paint_DrawString_EN(0, 0, "Free Play", &Font16, LIGHTBLUE, WHITE);
      Paint_DrawString_EN(0, 32, "Click to toggle metronome", &Font16, LIGHTBLUE, WHITE);
      Paint_DrawString_EN(0, 48, metronomeOn ? "Metronome: ON" : "Metronome: OFF", &Font16, LIGHTBLUE, WHITE);
      Paint_DrawString_EN(0, 64, "Left: exit", &Font16, LIGHTBLUE, WHITE);
      }
      break;
  

    case STATE_RUDIMENT_SELECT: {
      Serial.print("Current State: Rudiment Select\n");
        // replace these two lines w: 
        // int count = getFileCount("/rudiments");
      String fakeList[] = {"Paradiddle", "Flam", "Single Stroke", "Double Stroke"};
      int count = 4;

      if (needsRedraw) {
        LCD_Clear(0x07E0); // green
        Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0x07E0);
        Paint_DrawString_EN(0, 0, "Select Rudiment:", &Font16, GREEN, WHITE);

          for (int i = 0; i < count; i++) {
            String line = (i == rudimentIndex ? "> " : "  ") + fakeList[i];
            Paint_DrawString_EN(0, 16 + i * 16, line.c_str(), &Font16, 0x07E0, WHITE);
          }
      }
      // move the lines below into the if brackets
      // 
      // for (int i = 0; i < count; i++) {
      //   String name = getFileName("/rudiments", i);
      //   String line = (i == rudimentIndex ? "> " : "  ") + name;
      //   Paint_DrawString_EN(0, 16 + i * 16, line.c_str(), &Font16, WHITE, BLACK);
      // }

      MenuResult result = RudimentMenuUpdate();
      // if (menuWentBack) {
      //   menuWentBack = false;
      //   currentState = STATE_MAIN_MENU;
      // }

      // if (result != "") {
      //   //selectedRudiment = result;
      //   selectedRudiment = fakeList[rudimentIndex];
      //   currentState = STATE_TEMPO_SET;
      // }

      if (result == MENU_BACK) currentState = STATE_MAIN_MENU;
      if (result == MENU_SELECTED) {
        selectedRudiment = lastSelectedRudiment;
        currentState = STATE_TEMPO_SET;
      }
      break;
    }

    case STATE_TEMPO_SET: {
    if (needsRedraw) {
          LCD_Clear(0xF800); // red
          Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0xF800);
      Paint_DrawString_EN(0, 0, "Set Tempo:", &Font16, RED, WHITE);
    }
      char bpmStr[10];
      sprintf(bpmStr, "%d BPM", currentBPM);
      Paint_DrawString_EN(0, 32, bpmStr, &Font24, RED, WHITE);
      Paint_DrawString_EN(0, 100, "Click to confirm", &Font16, RED, WHITE);
      Paint_DrawString_EN(0, 116, "Left to go back", &Font16, RED, WHITE);
      TempoResult result = tempoUpdate();
      if (result == TEMPO_CONFIRMED) currentState = STATE_RUDIMENT_PREVIEW;
      if (result == TEMPO_BACK) currentState = STATE_RUDIMENT_SELECT;
      break;
    }

    case STATE_RUDIMENT_PREVIEW: {
    if (needsRedraw) {
          LCD_Clear(0xF800);
          Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0xF800);
      Paint_DrawString_EN(0, 0, "Listen:", &Font16, RED, WHITE);
      Paint_DrawString_EN(0, 16, selectedRudiment.c_str(), &Font16, RED, WHITE);
    }
      // TODO: audio partner plays the file here
      // when audio done → automatically move on
      delay(2000);
      currentState = STATE_RUDIMENT_LEADIN;
      break;
    }

    case STATE_RUDIMENT_LEADIN: {
      if (needsRedraw) {
          LCD_Clear(RED);
          Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0xF800);
         Paint_DrawString_EN(0, 0, "Get Ready...", &Font16, RED, WHITE);

         unsigned long beatInterval = 60000UL / lockedBPM;
         const char* counts[] = {"4", "3", "2", "1", "Go!"};
         for (int i = 0; i < 5; i++){
          Paint_DrawRectangle (0, 40, 60, 70, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
          Paint_DrawString_EN(0, 40, counts[i], &Font24, RED, WHITE);
          delay(beatInterval);
         }
        //  metro.setBeatsPerMinute(lockedBPM);
        //  metro.start();
        leadInCount = 0;
        // This doesn't have enough arguments or something funky
        // scoringInit();
        exerciseStartTime = millis();
        exerciseRun = true;
        currentState = STATE_RUDIMENT_PRACTICE;
        break;
      }
      // metro.setBeatsPerMinute(lockedBPM);
      // metro.start();
      // count 4 beats then start
  
    //   metro.check();
    //   if (metro.beat()){
    //   leadInCount++;
    //   }
      
    //   if (leadInCount >= 4) {
    //     leadInCount = 0;
    //     scoringInit();
    //     exerciseStartTime = millis();
    //     exerciseRun = true;
    //     currentState = STATE_RUDIMENT_PRACTICE;
  
    //   }
    //   break;
    // }

    case STATE_RUDIMENT_PRACTICE: {
      // countdown display
      unsigned long elapsed = millis() - exerciseStartTime;
      unsigned long remaining = (EXERCISE_LENGTH - elapsed) / 1000;
      // char timerStr[10];
      // sprintf(timerStr, "%lu sec", remaining);
      
      // if (needsRedraw) {
      //     LCD_Clear(0xF800);
      //     Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, RED);
      // Paint_DrawString_EN(0, 0, timerStr, &Font24, RED, WHITE);
      // Paint_DrawString_EN(0, 60, "Click: STOP", &Font16, RED, WHITE);
      // }
      static unsigned long lastRemaining = -1;
      if(remaining != lastRemaining){
        lastRemaining = remaining;
        char timerStr[10];
        sprintf(timerStr, "%lu", remaining);
        // LCD_Clear(0xF800);
        Paint_DrawString_EN (40, 0, "sec", &Font24, RED, WHITE);
        Paint_DrawRectangle(0, 0, 30, 30, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        Paint_DrawString_EN(0, 0, timerStr, &Font24, RED, WHITE);
        Paint_DrawString_EN(0, 60, "Click: STOP", &Font16, RED, WHITE);
      }
        // Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, RED);
        // Paint_DrawString_EN(0, 0, timerStr, &Font24, RED, WHITE);
        // Paint_DrawString_EN(0, 60, "Click: STOP", &Font16, RED, WHITE);
        // }
        
      metro.check();
      // track expected beat time
      if (metro.beat()) expectedBeatTime = millis();

      readSensor();
      if (hitAvailable()) {

        // This has too many arguments or something? I think I have the wrong version of some of the header files - Matt
        // processHit(latestHit.timestamp, latestHit.force, expectedBeatTime);
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
        // Too few arguments again from poor version control
        // scoringInit();
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
      char scoreStr[20];
      sprintf(scoreStr, "Score: %d%%", score);
      if (stateChanged) {
        currentOption = (MenuOption)1;
        Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0xF800);
        Paint_DrawString_EN(0, 0, scoreStr, &Font24, RED, WHITE);
      }
      if (needsRedraw) {
          // LCD_Clear(0xF800);
          // Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0xF800);
          // Paint_DrawString_EN(0, 0, scoreStr, &Font24, RED, WHITE);
           Paint_DrawString_EN(0, 60, currentOption == 0 ? "> Try Again" : "  Try Again", &Font16, RED, WHITE);
           Paint_DrawString_EN(0, 76, currentOption == 1 ? "> Exit" : "  Exit", &Font16, RED, WHITE);
      }
      menuUpdate();
      // Paint_DrawString_EN(0, 60, currentOption == 0 ? "> Try Again" : "  Try Again", &Font16, RED, WHITE);
      // Paint_DrawString_EN(0, 76, currentOption == 1 ? "> Exit" : "  Exit", &Font16, RED, WHITE);
     
      // if (digitalRead(JOY_SW) == LOW){
      // if(selected != NONE) {
      if (digitalRead(JOY_SW) == LOW){
          if (currentOption == 0) currentState = STATE_TEMPO_SET;
          if (currentOption == 1) currentState = STATE_RUDIMENT_SELECT;
      }     
      // Menudo i Option selected = menuUpdate();
      // if (selected != NONE) {
      //   if (currentOption == 0) currentState = STATE_TEMPO_SET;
      //   if (currentOption == 1) currentState = STATE_RUDIMENT_SELECT;
      // }
      break;
    }

    // case STATE_RESULTS:{
    //   static int cursor = 1;
    //   static int lastCursor = -1;
    //   static bool lastUp = HIGH;
    //   static bool lastDown = HIGH;
    //   static bool lastClick = HIGH;

    //   char scoreStr[20];
    //   sprintf(scoreStr, "Score: %d%%", score);

    //   if (stateChanged) {
    //     cursor = 1;
    //     lastCursor = -1;
    //     Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0xF800);
    //     Paint_DrawString_EN(0, 0, scoreStr, &Font24, RED, WHITE);
    //     cursor = 1;
    //   }

    //   bool up = digitalRead(JOY_VRX);
    //   bool down = digitalRead(JOY_VRY);
    //   bool click = digitalRead(JOY_SW);

    //   if (up == LOW && lastUp == HIGH) cursor = 0;
    //   if (down == LOW && lastDown == HIGH) cursor = 1;
      
    //   if (cursor != lastCursor){ 
    //     lastCursor = cursor; 
    //   Paint_DrawRectangle(0, 60, 200, 120, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    //   Paint_DrawString_EN(0, 60, currentOption == 0 ? "> Try Again" : "  Try Again", &Font16, RED, WHITE);
    //   Paint_DrawString_EN(0, 76, currentOption == 1 ? "> Exit" : "  Exit", &Font16, RED, WHITE);
    //   }

    //   if (click == LOW && lastClick == HIGH) {
    //     if (cursor == 0) currentState = STATE_TEMPO_SET;
    //         else currentState = STATE_RUDIMENT_SELECT;
    //   }

    //   lastUp = up;
    //   lastDown = down;
    //   lastClick = click; 
    //   break;
    // }

    case STATE_SOUND_LIBRARY: {
      if (needsRedraw) {
          LCD_Clear(0x780F); // purple
          Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0x780F);
      Paint_DrawString_EN(0, 0, "Sound Library:", &Font16, 0x780F, WHITE);
      }
      int count = getFileCount("/sound_library");
      for (int i = 0; i < count; i++) {
        String name = getFileName("/sound_library", i);
        String line = (i == activeSound ? "* " : "  ") + name;
        Paint_DrawString_EN(0, 16 + i * 16, line.c_str(), &Font16, 0x780F, WHITE);
      }

      MenuResult result = SoundMenuUpdate();
      // if (menuWentBack) {
      //   menuWentBack = false;
      //   currentState = STATE_MAIN_MENU;
      // }

      // if (result != "") currentState = STATE_MAIN_MENU;
      
      if (result == MENU_BACK) currentState = STATE_MAIN_MENU;
      if (result == MENU_SELECTED) {
        selectedSound = lastSelectedSound;
        currentState = STATE_MAIN_MENU;
        // currentState = STATE_TEMPO_SET;
      }

      break;
    }
  }}}
  


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