#include "Rudiments.h"
#include "AudioReader.h"

extern QueueHandle_t audioQueue;

// ==========================
const Rudiment* getRudiment(int index) {
  if (index < 0 || index >= rudimentCount) return nullptr;
  return &rudiments[index];
}

int getRudimentCount() {
  return rudimentCount;
}

void playRudiment(Rudiment rudiment, int bpm, const char* audioFile) {
  // Calculate the duration of one 16th note in milliseconds
  // 60,000 ms / (BPM * 4 steps per beat)
  float msPerStep = 60000.0 / (bpm * 4.0);

  int lastStep = 0;

  for (int i = 0; i < rudiment.length; i++) {
    RudimentNote note = rudiment.pattern[i];

    // Calculate delay based on the difference between this step and the last
    // This allows for non-consecutive notes (rests)
    if (i > 0) {
      int stepDifference = note.step - lastStep;
      if (stepDifference > 0) {
        delay(stepDifference * msPerStep);
      }
    }

    // Handle Volume logic
    // float baseVol = getVolume(); 
    float baseVol = 0.1;
    
    // Cap base volume at 0.8 to ensure overhead for the 20% accent
    if (baseVol > 0.5f) baseVol = 0.5f;

    float finalVol;
    if(note.accent) {
      finalVol = baseVol + 0.5;
      Serial.println("This note is accented");
    } else {
      finalVol = baseVol;
    }

    // Trigger the audio
    play_wav_from_sd(audioFile, finalVol);

    // AudioRequest req;
    // strncpy(req.path, audioFile, sizeof(req.path));
    // req.volume = finalVol;
    // xQueueSend(audioQueue, &req, portMAX_DELAY);

    lastStep = note.step;
  }
}

Rudiment getRudimentByName(String selectedName) {
  // Calculate the number of items in the array
  int totalRudiments = sizeof(rudiments) / sizeof(rudiments[0]);

  for (int i = 0; i < totalRudiments; i++) {
    // Arduino String objects have an .equals() method for easy comparison
    if (selectedName.equals(rudiments[i].name)) {
      return rudiments[i];
    }
  }

  // Fallback: return the first rudiment if no match is found
  return rudiments[0];
}