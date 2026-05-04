#include "Menu.h"
#include "SDReader.h"
#include "Controls.h"
#include "HitDetection.h"
#include "LEDfeedback.h"
#include "Scoring.h"
#include "metronome.h"
#include "Rudiments.h"
#include "LCD_Driver.h"
#include "led_driver.h"
#include "GUI_Paint.h"
#include "AudioReader.h"
#include <SPI.h>

// pin assignments
// LED Strip Pins
#define STRIP_SDI_PIN 2
#define STRIP_CLK_PIN 0
#define STRIP_LATCH_PIN 4
#define STRIP_OE_PIN 16

// LED ring Pins
#define RING_SDI_PIN 2
#define RING_CLK_PIN 0
#define RING_LATCH_PIN 12
#define RING_OE_PIN 39

// #define LED_RING_PIN 99
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
#define RING_SDI_PIN STRIP_SDI_PIN
#define RING_CLK_PIN STRIP_CLK_PIN
#define RING_LATCH_PIN 33
#define RING_OE_PIN -1
//#define DATA_PIN 2
//#define CLOCK_PIN 3
//#define LATCH_PIN 4
//#define OE_PIN 5

//led ring 
//#define RING_LATCH_PIN 
//#define RING_OE_PIN

Metronome metro(SPEAKER_PIN);
QueueHandle_t audioQueue;
LED_Driver* stripDriver;
LED_Driver* ringDriver;

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
String selectedSound = "/sound_library/snare_test_2.wav";
int score = 0;
unsigned long expectedBeatTime = 0;
int leadInCount = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup started");
  
  // LCD
  Config_Init();
  Serial.println("Config started");
  LCD_Init();
  Serial.println("Init started");
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, ROTATE_90, BLACK); 

  // Beginning Audio Queue
  audioQueue = xQueueCreate(10, sizeof(AudioRequest));
  xTaskCreatePinnedToCore(audioManagerTask, "AudioMgr", 10240, NULL, 5, NULL, 0);
  

  // stripDriver = new LED_Driver(STRIP_SDI_PIN, STRIP_CLK_PIN, STRIP_LATCH_PIN, STRIP_OE_PIN);
  ringDriver = new LED_Driver(RING_SDI_PIN, RING_CLK_PIN, RING_LATCH_PIN, RING_OE_PIN);
  ringDriver->writePattern(0b11111111);
  // stripDriver->startupSequence();
  // stripDriver->writePattern(0xFF);
  // delay(500);
  // stripDriver->writePattern(0XF0);
  // delay(500);
  // stripDriver->writePattern(0x0F);

  SPIClass* vspi = sdInit(VSPI_CLK_PIN, VSPI_CIPO_PIN, VSPI_COPI_PIN, VSPI_CS_PIN);
  menuInit();
  setupLedStrip();
  // controlsInit();
  hitDetectionInit();
  // setupLedStrip();
  // setupRing();

  // metronome
  metro.begin();
  // metro.setMeasure(4);

}

State lastState = STATE_POWER_ON;
MenuOption lastOption = FREE_PLAY;
bool lastMetronomeOn = false;
int lastRudimentIndex = 0;
int lastSoundIndex = 0;
int lastActiveSound = 0;
int lastBPM = 0;
static int beatCount = 0;
unsigned long lastBeatTime = 0;
State previousState = STATE_MAIN_MENU;

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
                || (currentOption != lastOption); 
lastState = currentState;
lastOption = currentOption;
lastMetronomeOn = metronomeOn;

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
      // Serial.print("Current State: Menu\n");
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

    case STATE_FREE_PLAY: {
      Serial.print("Current State: Freeplay\n");
      metro.setBeatsPerMinute(currentBPM);
      if (metronomeOn) {
        if (!metro.isRunning()) metro.start();
        metro.check();
        unsigned long beatIntervalMs = 60000UL / lockedBPM;
        if (millis() - lastBeatTime >= beatIntervalMs) {
         lastBeatTime = millis();
         play_wav_from_sd("/sound_library/side_stick_test.wav", 1.0);
}
        // if (metro.isOnBeat()){
        //   play_wav_from_sd(selectedSound.c_str(), 1.0);
        //   }
      } else {
        metro.stop();
      }
      // Hit hit = waitForHit(PIEZO_PIN, 0.5, millis() + 150);
      // if (hit.timestamp > 0) {
      //  showIntensity(hit.voltage);
      // }

      FreePlayResult fp = freePlayUpdate();
      if (fp == FREEPLAY_EXIT){
        metro.stop();
        metronomeOn = false;
        clearFeedback();
        currentState = STATE_MAIN_MENU;
        break;
      }
      if (fp == FREEPLAY_SET_TEMPO){
        previousState = STATE_FREE_PLAY;
        currentState = STATE_TEMPO_SET;
      }

      // if (!freePlayUpdate()) {
      //   metro.stop();
      //   metronomeOn = false;
      //   clearFeedback();
      //   // LCD_Clear(0xF800);
      //   currentState = STATE_MAIN_MENU;
      // }
      
       if (needsRedraw) {
          LCD_Clear(LIGHTBLUE); // blue
          Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0x001F);
     
      Paint_DrawString_EN(0, 0, "Free Play", &Font16, LIGHTBLUE, WHITE);
      Paint_DrawString_EN(0, 32, "Click to toggle metronome", &Font16, LIGHTBLUE, WHITE);
      Paint_DrawString_EN(0, 48, metronomeOn ? "Metronome: ON" : "Metronome: OFF", &Font16, LIGHTBLUE, WHITE);
      Paint_DrawString_EN(0, 64, "Left: exit", &Font16, LIGHTBLUE, WHITE);
      Paint_DrawString_EN(0,100, "Selected Tempo:", &Font16, LIGHTBLUE, WHITE);
      Paint_DrawString_EN(60, 120, "BPM", &Font24, LIGHTBLUE, WHITE);
      

        needsRedraw = false;
        lastMetronomeOn = metronomeOn;
        lastBPM = -1;
       }

      // if (needsRedraw && metronomeOn){
      //   metro.start();
      // }
      if(metronomeOn && !metro.isRunning()){
        metro.start();
      }

      if (metronomeOn != lastMetronomeOn){
        Paint_DrawRectangle(0, 48, LCD_WIDTH, 48 + Font16.Height, LIGHTBLUE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        Paint_DrawString_EN(0, 48, metronomeOn ? "Metronome: ON" : "Metronome: OFF", &Font16, LIGHTBLUE, WHITE);
        lastMetronomeOn = metronomeOn;
      }

      if (abs(currentBPM - lastBPM) > 1){
      char bpmStr[10];
      sprintf(bpmStr, "%d", currentBPM);
      Paint_DrawRectangle(0, 120, 55, 100 + Font24.Height, 0xF800, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      Paint_DrawString_EN(0, 120, bpmStr, &Font24, LIGHTBLUE, WHITE);
      lastBPM = currentBPM;
      }
      break;
  }
  
    case STATE_RUDIMENT_SELECT: {
      Serial.print("Current State: Rudiment Select\n");
        // replace these two lines w: 
        // int count = getFileCount("/rudiments");
      //String fakeList[] = {"Paradiddle", "Flam", "Single Stroke", "Double Stroke"};
      int count = getRudimentCount();
      bool listChanged = (rudimentIndex != lastRudimentIndex);

      if (needsRedraw) {
        LCD_Clear(0x07E0); // green
        Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0x07E0);
        Paint_DrawString_EN(0, 0, "Select Rudiment:", &Font16, GREEN, WHITE);
      }

      if (needsRedraw || listChanged) {
        for (int i = 0; i < count; i++) {
            const Rudiment* r = getRudiment(i);
            String line = (i == rudimentIndex ? "> " : "  ") + String(r->name);
            Paint_DrawString_EN(0, 16 + i * 16, line.c_str(), &Font16, 0x07E0, WHITE);
           }
          lastRudimentIndex = rudimentIndex;
      }

      MenuResult result = RudimentMenuUpdate();

      if (result == MENU_BACK) currentState = STATE_MAIN_MENU;
      if (result == MENU_SELECTED) {
        previousState = STATE_RUDIMENT_SELECT;
        const Rudiment* r = getRudiment(rudimentIndex);
        currentPattern = r->pattern;
        patternLength = r->length;
        selectedRudiment = r->name;
        currentState = STATE_TEMPO_SET;
      }
      break;
    }

    case STATE_TEMPO_SET: { 
    if (needsRedraw) {
          LCD_Clear(0xF800); // red
          Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0xF800);
          Paint_DrawString_EN(0, 0, "Set Tempo:", &Font16, RED, WHITE);
          Paint_DrawString_EN(60, 32, "BPM", &Font24, RED, WHITE);
          Paint_DrawString_EN(0, 100, "Click to confirm", &Font16, RED, WHITE);
          Paint_DrawString_EN(0, 116, "Left to go back", &Font16, RED, WHITE);
    }

    if (abs(currentBPM - lastBPM) > 1){
      char bpmStr[10];
      sprintf(bpmStr, "%d", currentBPM);
      Paint_DrawRectangle(0, 32, 55, 32 + Font24.Height, 0xF800, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      Paint_DrawString_EN(0, 32, bpmStr, &Font24, RED, WHITE);
      lastBPM = currentBPM;
    }
      TempoResult result = tempoUpdate();
      Serial.print("previousState: ");
      Serial.println(previousState);
      if (result == TEMPO_CONFIRMED){
        if (previousState == STATE_FREE_PLAY) currentState = STATE_FREE_PLAY;
        else currentState = STATE_RUDIMENT_PREVIEW;
      } 
      if (result == TEMPO_BACK){
        if (previousState == STATE_FREE_PLAY) currentState = STATE_FREE_PLAY;
        else currentState = STATE_RUDIMENT_SELECT;
      }
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
      Rudiment selected = getRudimentByName(selectedRudiment);
      playRudiment(selected, lockedBPM, selectedSound.c_str());
      delay(selected.length * (60000 / (lockedBPM * 4)) + 500); 
      currentState = STATE_RUDIMENT_LEADIN;
      break;
    }

    case STATE_RUDIMENT_LEADIN: {
      if (stateChanged){
        metro.setBeatsPerMinute(lockedBPM);
        LCD_Clear(RED);
          Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0xF800);
         Paint_DrawString_EN(0, 0, "Get Ready...", &Font16, RED, WHITE);

         unsigned long beatInterval = 60000UL / lockedBPM;
         const char* counts[] = {"4", "3", "2", "1", "Go!"};
         for (int i = 0; i < 5; i++){
          Paint_DrawRectangle (0, 40, 60, 70, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
          Paint_DrawString_EN(0, 40, counts[i], &Font24, RED, WHITE);
          play_wav_from_sd("/sound_library/side_stick_test.wav", 1.0);
          delay(beatInterval);
      }
      // if (needsRedraw) {
      //     metro.check();
      //     Serial.print("Current Time: ");
      //     Serial.println(millis());
      //     LCD_Clear(RED);
      //     Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0xF800);
      //    Paint_DrawString_EN(0, 0, "Get Ready...", &Font16, RED, WHITE);

      //    unsigned long beatInterval = 60000UL / lockedBPM;
      //    const char* counts[] = {"4", "3", "2", "1", "Go!"};
      //    for (int i = 0; i < 5; i++){
      //     Paint_DrawRectangle (0, 40, 60, 70, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
      //     Paint_DrawString_EN(0, 40, counts[i], &Font24, RED, WHITE);
      //     play_wav_from_sd("/sound_library/side_stick_test.wav", 1.0);
      //     delay(beatInterval);
      //    }
        //  metro.setBeatsPerMinute(lockedBPM);
        //  metro.start();
        metro.stop();
        beatCount = 0;
        leadInCount = 0;
        scoringInit(lockedBPM);
        exerciseStartTime = millis();
        exerciseRun = true;
        metro.setBeatsPerMinute(lockedBPM);
        metro.start();
        Serial.print("Lead-in ending, BPM set to: ");
        Serial.println(metro.getBeatsPerMinute());
        currentState = STATE_RUDIMENT_PRACTICE;
      }
      break;
    }

    case STATE_RUDIMENT_PRACTICE: {
      // countdown display
      if (stateChanged){
         beatCount = 0;
         lastBeatTime = millis();
       }

      //  metro.check();
      //  if (metro.isOnBeat()) {
      //   play_wav_from_sd("/sound_library/side_stick_test.wav", 1.0);
      //     }

      unsigned long beatIntervalMs = 60000UL / lockedBPM;
    if (millis() - lastBeatTime >= beatIntervalMs) {
      lastBeatTime += beatIntervalMs;
      play_wav_from_sd("/sound_library/side_stick_test.wav", 1.0);
      
}

  //      metro.check();
  //      if (metro.beat()) {
  //          beatCount++;
  //       if (beatCount % 2 == 0) {  // every other fire = true BPM
  //        play_wav_from_sd("/sound_library/side_stick_test.wav", 1.0);
  //   }
  // } 
    Hit hit = waitForHit(PIEZO_PIN, 0.5, millis() + 2);
      if (hit.timestamp > 0) {
        // clearHit();
        unsigned long expectedTime = patternStartTime + currentPattern[currentNoteIndex].step * subdivisionMs;
        long error = (long)hit.timestamp - (long)expectedTime;
        int forceADC = (int)(hit.voltage * 4096 / 3.3);
        HitResult result = processHit(hit.timestamp, forceADC);
        // if (result == HIT_GOOD || result == HIT_IGNORED) {
        //   updateMissedNotes();  // only check for misses if hit didn't land near this note
        // }
        if (result == HIT_GOOD) showAccuracy(error, FEEDBACK_TIMED);
        if (result == HIT_EXTRA) showAccuracy(0, FEEDBACK_EXTRA);
        showIntensity(hit.voltage);
        } else {
        updateMissedNotes();  // check for missed notes even when no hit
      }

      unsigned long elapsed = millis() - exerciseStartTime;
      unsigned long remaining = (EXERCISE_LENGTH - elapsed) / 1000;

      static unsigned long lastRemaining = -1;
      if(remaining != lastRemaining){
        lastRemaining = remaining;
        char timerStr[10];
        // sprintf(timerStr, "%lu", remaining);
        // LCD_Clear(0xF800);
        // Paint_DrawString_EN (40, 0, "sec", &Font24, RED, WHITE);
        // Paint_DrawRectangle(0, 0, 30, 30, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        // Paint_DrawString_EN(0, 0, timerStr, &Font24, RED, WHITE);
        // Paint_DrawString_EN(0, 60, "Click: STOP", &Font16, RED, WHITE);
      }
        
      // metro.check();
      // if (metro.beat() && metro._tick == HIGH){
      //     Serial.print("Playing metronome sound: ");
      //     Serial.println(selectedSound.c_str());
      //     play_wav_from_sd("/sound_library/side_stick_test.wav", 1.0);
      //   }

      // track expected beat time
      // if (metro.beat()) expectedBeatTime = millis();
      // if (metro.beat()){ // THIS MIGHT BE BREAKING THE SOUND
      //   AudioRequest req;
      //   strncpy(req.path, "/sounds/snare_test_1.wav", sizeof(req.path));
      //   req.volume = 1.0;
      //   xQueueSend(audioQueue, &req, 0);
      // }

      // metro.check();
      // if (metro.isOnBeat()){
      //   play_wav_from_sd("/sound_library/side_stick_test.wav", 1.0);
      // }


      // Waiting for the next hit
      
    
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
        scoringInit(lockedBPM);
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
  
      if (digitalRead(JOY_SW) == LOW){
          if (currentOption == 0) currentState = STATE_TEMPO_SET;
          if (currentOption == 1) currentState = STATE_RUDIMENT_SELECT;
      }     
      break;
    }


    case STATE_SOUND_LIBRARY: {
      int count = getFileCount("/sound_library");
      bool listChanged = (soundIndex != lastSoundIndex);
      bool selectionChanged = (activeSound != lastActiveSound);

      if (needsRedraw) {
          LCD_Clear(0x780F); // purple
          Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, 0x780F);
      Paint_DrawString_EN(0, 0, "Sound Library:", &Font16, 0x780F, WHITE);
      }

      if (needsRedraw || listChanged || selectionChanged){
        for (int i = 0; i < count; i++) {
        String name = getFileName("/sound_library", i);
        String prefix;
        if (i == soundIndex && i == activeSound) prefix = ">*";
        else if (i == soundIndex) prefix = "> ";
        else if (i == activeSound) prefix = " *";
        else prefix = "  ";
        String line = prefix + name;
        // String line = (i == soundIndex ? "> " : "  ") + getFileName("/sound_library", i);
        Paint_DrawString_EN(0, 16 + i * 16, line.c_str(), &Font16, 0x780F, WHITE);
      }
      lastSoundIndex = soundIndex;
      lastActiveSound = activeSound;
      }

      MenuResult result = SoundMenuUpdate();
      
      if (result == MENU_BACK) currentState = STATE_MAIN_MENU;
      if (result == MENU_SELECTED) {
        selectedSound = lastSelectedSound;
        Serial.print("Selected Sound: ");
        Serial.println(selectedSound.c_str());
      }
      break;
    }
  }
}
