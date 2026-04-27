#ifndef SCORING_H
#define SCORING_H
#include <Arduino.h>

// define RudimentNote
struct RudimentNote {
  int step; // the note's position in a 16th note grid
  bool accent; // is the note accented or not
};

// define HitResult
enum HitResult {
  HIT_GOOD,
  HIT_EXTRA,
  HIT_IGNORED  // too far out even for extra hit zone
};

// variables
extern int totalHits;
extern int correctHits;
extern unsigned long exerciseStartTime;
extern bool exerciseRun;

  // pattern tracking
extern const RudimentNote* currentPattern;
extern int patternLength;
extern int currentNoteIndex;
extern unsigned long patternStartTime;

  // timing
extern int subdivisionMs;

// constants
const unsigned long EXERCISE_LENGTH = 30000;
const unsigned long FEEDBACK_DURATION_MS = 500;
const int HIT_WINDOW_MS = 40;
// hard vs soft hit threshold for accents (this is the ADC value, not the voltage)
// NEEDS TO BE CHANGED BASED ON TESTING
#define ACCENT_THRESHOLD 2500 // should be around 0.8V

// functions
void scoringInit(int bpm);
HitResult processHit(unsigned long hitTime, int force);
void updateMissedNotes();
int getScore();
bool exerciseFinished();
void runExerciseTimer();

#endif