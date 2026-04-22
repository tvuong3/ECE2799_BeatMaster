#ifndef MENU_H
#define MENU_H
#include <Arduino.h>


#define JOY_VRX  34
#define JOY_VRY  35
#define JOY_SW 32
#define TEMPO_PIN 33

// main menu options
enum MenuOption {
  NONE,
  FREE_PLAY,
  RUDIMENTS,
  SOUND_LIBRARY
};

// variables
extern MenuOption currentOption;
extern int rudimentIndex;
extern int soundIndex;
extern int activeSound;
extern bool metronomeOn;

// functions
void menuInit();
MenuOption menuUpdate();
String RudimentMenuUpdate();
String SoundMenuUpdate();
bool freePlayUpdate();

// tempo options
enum TempoResult {
  TEMPO_STILL_SETTING,
  TEMPO_CONFIRMED,
  TEMPO_BACK
};

// tempo variables
extern int currentBPM;
extern int lockedBPM;

// function
TempoResult tempoUpdate();


#endif