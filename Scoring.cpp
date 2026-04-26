#include "Scoring.h"
#include "Arduino.h"
#include <stdlib.h>

// HOW THIS WORKS
// Metronome -> sets BPM
// BPM -> defines a grid, like every 125ms or something
// Rudiment -> says which grid points matter
// User hit -> compare to the closest grid point
// Difference -> used to calculate score



// initialize variables
int totalHits = 0;
int correctHits = 0;
unsigned long exerciseStartTime = 0;
bool exerciseRun = false;
const RudimentNote* currentPattern = nullptr;
int patternLength = 0;
int currentNoteIndex = 0;
unsigned long patternStartTime = 0;

int subdivisionMs = 0; // duration of one 16th note

// init scoring
void scoringInit(int bpm) {
  totalHits = 0;
  correctHits = 0;
  exerciseRun = true;
  exerciseStartTime = millis();
  currentNoteIndex = 0;
  patternStartTime = millis();
  subdivisionMs = (60000 / bpm) / 4; // 16th note duration (time between each subdivision)
}

// process a hit
void processHit(unsigned long hitTime, int force) {
  if (currentPattern == nullptr) return;

  // expected time of current note
  unsigned long expectedTime = patternStartTime + currentPattern[currentNoteIndex].step * subdivisionMs;

  long error = (long)hitTime - (long)expectedTime;
  long absError = abs(error);

  // defime the timing windows to match the LED thresholds
  int points = 0;
  points = max(0, points - 30);
  bool matched = false;

  if (absError <= 30) {
    points = 100; // perfect timing, green LED
    matched = true;
  }
  else if (absError <= 60) {
    points = 70; // slightly off, first yellow LED
    matched = true;
  }
  else if (absError <= 120) {
    points = 40; // very off, second yellow LED
    matched = true;
  }

  // CASES

  // case 1: 
  if (matched) {

    // check for accents
    if (currentPattern[currentNoteIndex].accent) {
      if (force < ACCENT_THRESHOLD) {
        points -= 20; // penalize weak accent
      }
    }

    correctHits += points;
    totalHits += 100;

    // move to next note
    currentNoteIndex++;

    // loop pattern
    if (currentNoteIndex >= patternLength) {
      currentNoteIndex = 0;
      patternStartTime = millis();
    }

    return;
  }

  // case 2: 
  if (absError > 120) { // penalize extra hit, but DO NOT shift the pattern
    correctHits += 20;
    totalHits +=100;
    return;
  }    
}

void updateMissedNotes() {
  if (currentPattern == nullptr) return;

  unsigned long now = millis();

  unsigned long expectedTime = patternStartTime + currentPattern[currentNoteIndex].step * subdivisionMs;

  // if we've passed the window and no hit occurred
  //if ((long)(now - expectedTime) > 120) {
  while ((long)(now - expectedTime) > 120) {

    // missed note penalty
    correctHits += 0;
    totalHits += 100;

    // move on WITHOUT requiring a hit to not mess up the pattern
    currentNoteIndex++;

    if (currentNoteIndex >= patternLength) {
      currentNoteIndex = 0;
      patternStartTime = millis();
      break;
    }

    expectedTime = patternStartTime + currentPattern[currentNoteIndex].step * subdivisionMs;
  }
}

int getScore() {
  if (totalHits == 0) return 0;
  return (correctHits * 100) / totalHits;
}

bool exerciseFinished() {
  return !exerciseRun;
}

// timer
void runExerciseTimer() {
  if (millis() - exerciseStartTime >= EXERCISE_LENGTH) {
    exerciseRun = false;
  }
}