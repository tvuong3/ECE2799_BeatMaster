#include "Scoring.h"
#include "Arduino.h"

// initialize variables
int totalHits = 0;
int correctHits = 0;
unsigned long exerciseStartTime = 0;
bool exerciseRun = false;

void scoringInit() {
  totalHits = 0;
  correctHits = 0;
  exerciseRun = false;
}

void processHit(unsigned long hitTime, int force, unsigned long expectedBeatTime) {
  totalHits++;
  long error = (long)hitTime - (long)expectedBeatTime;
  if (abs(error) <= HIT_WINDOW_MS) {
    correctHits++;
  }
}

int getScore() {
  if (totalHits == 0) return 0;
  return (correctHits * 100) / totalHits;
}

bool exerciseFinished() {
  return !exerciseRun;
}

void runExerciseTimer() {
  if (millis() - exerciseStartTime >= EXERCISE_LENGTH) {
    exerciseRun = false;
  }
}