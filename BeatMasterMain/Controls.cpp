#include "Controls.h"
#include <Arduino.h>

#define DRUM_VOL_PIN 36
#define METRO_VOL_PIN 39

// define variables
int drumVolume = 0;
int metronomeVolume = 0;


// sets up pins and reads initial values
void controlsInit(){
  pinMode(DRUM_VOL_PIN, INPUT);
  pinMode(METRO_VOL_PIN, INPUT);
  drumVolume = map(analogRead(DRUM_VOL_PIN), 0, 4095, 0, 255);
  metronomeVolume = map(analogRead(METRO_VOL_PIN), 0, 4095, 0, 255);

}


// reads both dials and updates variables
void controlsUpdate(){
  drumVolume = map(analogRead(DRUM_VOL_PIN), 0, 4095, 0, 255);
  metronomeVolume = map(analogRead(METRO_VOL_PIN), 0, 4095, 0, 255);
}