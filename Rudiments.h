#ifndef RUDIMENTS_H
#define RUDIMENTS_H

#include "Scoring.h"

struct Rudiment {
  const char* name;
  const RudimentNote* pattern;
  int length;
};

const Rudiment* getRudiment(int index);
int getRudimentCount();

#endif