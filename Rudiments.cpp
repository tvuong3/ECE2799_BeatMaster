#include "Rudiments.h"

// define all rudiments

// Single Stroke (R L R L)
const RudimentNote singleStroke[] = {
  {0, false}, {1, false}, {2, false}, {3, false}
};

// Double Stroke (R R L L)
const RudimentNote doubleStroke[] = {
  {0, false}, {0, false}, {2, false}, {2, false}
};

// Paradiddle (R L R R L R L L)
const RudimentNote paradiddle[] = {
  {0, true}, {1, false}, {2, false}, {3, false},
  {4, true}, {5, false}, {6, false}, {7, false}
};

// Flam
const RudimentNote flam[] {
  {0, false}, {0, true}
};

// Flam Tap
const RudimentNote flamTap[] = {
  {0, false}, {0, true}, {2, false}, {3, false}
};

// Paradiddle-Diddle
const RudimentNote paradiddleDiddle[] = {
  {0, true}, {1, false}, {2, false},
  {3, false}, {4, false}, {5, false}
};

// ==========================
// LIST
// ==========================
const Rudiment rudiments[] = {
  {"Single Stroke", singleStroke, 4},
  {"Double Stroke", doubleStroke, 4},
  {"Paradiddle", paradiddle, 8},
  {"Flam", flam, 2},
  {"Flam Tap", flamTap, 4},
  {"Paradiddle Diddle", paradiddleDiddle, 6}
};

const int rudimentCount = sizeof(rudiments) / sizeof(Rudiment);

// ==========================
const Rudiment* getRudiment(int index) {
  if (index < 0 || index >= rudimentCount) return nullptr;
  return &rudiments[index];
}

int getRudimentCount() {
  return rudimentCount;
}