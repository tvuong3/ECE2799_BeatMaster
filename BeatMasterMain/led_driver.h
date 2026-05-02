#ifndef LED_DRIVER_LIB
#define LED_DRIVER_LIB

#include <Arduino.h>
#include <SPI.h>

class LED_Driver {
  public:
    int sdiPin = -1;
    int clkPin = -1;
    int latchPin = -1;
    int oePin = -1;
    LED_Driver(int sdiPinIn, int clkPinIn, int latchPinIn, int oePinIn);
    void writePattern(uint8_t pattern);
    void lightLED(int index);
    void setBrightness(int brightness); // Takes brightness as a percentage
    void testHigh(int pin);
    void startupSequence();
};

#endif