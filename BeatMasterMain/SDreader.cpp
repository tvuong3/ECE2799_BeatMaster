#include <SD.h>
#include "SDreader.h"

// sets up SD card once at startup
void sdInit() {
  if (!SD.begin(SD_PIN)) {
    Serial.println("SD card failed to initialize.");
    return;
  }
  Serial.println("SD card initialized.");
}

// returns how many files are in a folder
int getFileCount(const char* inputPath){
  int count = 0;

  File folder = SD.open(inputPath);
  // error checking
  if (!folder) {
    Serial.println("Failed to open folder.");
    return 0;
  }

  File file = folder.openNextFile();

  while (file == true) {
    count++;
    file = folder.openNextFile();
  }
  return count;
}


// reads all filenames from a folder and returns them as a list
String getFileName(const char* inputPath, int index){
  File folder = SD.open(inputPath);

  // error checking
  if (!folder) {
    Serial.println("Failed to open folder.");
    return "";
  }
  if (index >= getFileCount(inputPath) || index < 0) {
    Serial.println("Index out of range.");
    return "";
  }
  
  File file = folder.openNextFile();
  for (int i = 0; i < index; i++) {
    file = folder.openNextFile();
  }
  return file.name();
}