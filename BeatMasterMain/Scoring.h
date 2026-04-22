#ifndef SCORING_H
#define SCORING_H

// variables
extern int totalHits;
extern int correctHits;
extern unsigned long exerciseStartTime;
extern bool exerciseRun;

// constants
const unsigned long EXERCISE_LENGTH = 30000;
const unsigned long FEEDBACK_DURATION_MS = 500;
const int HIT_WINDOW_MS = 30;

// functions
void scoringInit();
void processHit(unsigned long hitTime, int force, unsigned long expectedBeatTime);
int getScore();
bool exerciseFinished();
void runExerciseTimer();

#endif