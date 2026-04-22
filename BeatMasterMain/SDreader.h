#ifndef SDREADER_H
#define SDREADER_H

#include <Arduino.h>

#define SD_PIN 5
#define MAX_FILES 20

void sdInit();
int getFileCount(const char* inputPath);
String getFileName(const char* inputPath, int index);

#endif