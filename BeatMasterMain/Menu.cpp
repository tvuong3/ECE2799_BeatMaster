#include "Menu.h"
#include "SDReader.h"


// define variables
MenuOption currentOption = FREE_PLAY;
int rudimentIndex = 0;
int soundIndex = 0;
int activeSound = 0;
bool metronomeOn = false;
int currentBPM = 0;
int lockedBPM = 0;

void menuInit(){
  pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(TEMPO_PIN, INPUT);
  currentBPM = map(analogRead(TEMPO_PIN), 0, 4095, 40, 200);
  lockedBPM = currentBPM;
}

MenuOption menuUpdate(){
  
  int x = analogRead(JOY_VRX);
  int y = analogRead(JOY_VRY);
  int btn = digitalRead(JOY_SW);
  
  // go up
  if (y < 1000 && currentOption > 0) {
    currentOption = (MenuOption)(currentOption - 1);
    delay(200);
}
  // go down
  if (y > 3000 && currentOption < SOUND_LIBRARY) {
    currentOption = (MenuOption)(currentOption + 1);
    delay(200);
  }
  // go back
  if (x < 1000) {
    currentOption = FREE_PLAY;
    delay(200);
    return NONE;
  } 
  // select
  if (x > 3000 || btn == LOW) {
    delay(200);
    return currentOption;
  } 
  // nothing happened
  return NONE;
}


String RudimentMenuUpdate(){
  int x = analogRead(JOY_VRX);
  int y = analogRead(JOY_VRY);
  int btn = digitalRead(JOY_SW);

  // go up
  if (y < 1000 && rudimentIndex > 0) {
    rudimentIndex = rudimentIndex - 1;
    delay(200);
}
  // go down
  if (y > 3000 && rudimentIndex < getFileCount("/rudiments") - 1) {
    rudimentIndex = rudimentIndex + 1;
    delay(200);
  }
  // go back
  if (x < 1000) {
    rudimentIndex = 0;
    delay(200);
    return "";
  } 
  // select
  if (x > 3000 || btn == LOW) {
    delay(200);
    return getFileName("/rudiments", rudimentIndex);
  } 
  // nothing happened
  return "";
}



String SoundMenuUpdate(){
  int x = analogRead(JOY_VRX);
  int y = analogRead(JOY_VRY);
  int btn = digitalRead(JOY_SW);

  // go up
  if (y < 1000 && soundIndex > 0) {
    soundIndex = soundIndex - 1;
    delay(200);
}
  // go down
  if (y > 3000 && soundIndex < getFileCount("/soundlibrary") - 1) {
    soundIndex = soundIndex + 1;
    delay(200);
  }
  // go back
  if (x < 1000) {
    soundIndex = 0;
    delay(200);
    return "";
  } 
  // select
  if (x > 3000 || btn == LOW) {
    delay(200);
    activeSound = soundIndex;
    return getFileName("/soundlibrary", soundIndex);
  } 
  // nothing happened
  return "";
}

bool freePlayUpdate(){
  int x = analogRead(JOY_VRX);
  int btn = digitalRead(JOY_SW);

  // go back
  if (x < 1000) {
    delay(200);
    return false;  
  }

  // metronome toggle
  if (btn == LOW) {
    delay(200);
    metronomeOn = !metronomeOn;
  }

  return true;
}


TempoResult tempoUpdate(){
  int btn = digitalRead(JOY_SW);
  int x = analogRead(JOY_VRX);

  currentBPM = map(analogRead(TEMPO_PIN), 0, 4095, 40, 200);
  
  // go back
  if (x < 1000) {
    delay(200);
    return TEMPO_BACK;
  }
  
  // confirm
  if (btn == LOW) {
    lockedBPM = currentBPM;
    delay(200);
    return TEMPO_CONFIRMED;
  }
  
  return TEMPO_STILL_SETTING;

}