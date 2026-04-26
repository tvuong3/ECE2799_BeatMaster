#ifndef SDREADER_H
#define SDREADER_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

SPIClass* sdInit(int clkPin, int cipoPin, int copiPin, int csPin);
int getFileCount(const char* inputPath);
String getFileName(const char* inputPath, int index);

#endif