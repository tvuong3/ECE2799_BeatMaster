#include "led_driver.h"

#define STRIP_SDI_PIN 10
#define STRIP_CLK_PIN 9
#define STRIP_LATCH_PIN 8
#define STRIP_OE_PIN 7

LED_Driver* stripDriver;

void setup() {
  // put your setup code here, to run once:
  stripDriver = new LED_Driver(STRIP_SDI_PIN, STRIP_CLK_PIN, STRIP_LATCH_PIN, STRIP_OE_PIN);
  Serial.begin(9600);

  // TEST
  // digitalWrite(STRIP_SDI_PIN, HIGH);
  // digitalWrite(STRIP_CLK_PIN, HIGH);
  // digitalWrite(STRIP_LATCH_PIN, HIGH);
  // digitalWrite(STRIP_OE_PIN, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Test Beginning\n");
  stripDriver->setBrightness(10);
  stripDriver->lightLED(0);
  delay(200);
  stripDriver->setBrightness(20);
  stripDriver->lightLED(1);
  delay(200);
  stripDriver->setBrightness(30);
  stripDriver->lightLED(2);
  delay(200);
  stripDriver->setBrightness(40);
  stripDriver->lightLED(3);
  delay(200);
  stripDriver->setBrightness(50);
  stripDriver->lightLED(4);
  delay(200);
  stripDriver->setBrightness(60);
  stripDriver->lightLED(5);
  delay(200);
  stripDriver->setBrightness(70);
  stripDriver->lightLED(6);
  delay(200);
  stripDriver->setBrightness(100);
  stripDriver->lightLED(7);
  delay(200);
  // int testNum = stripDriver->sdiPin;
  // Serial.print(testNum);
  // delay(1000);
  // stripDriver->setBrightness(50);
  // delay(1000);
  // stripDriver->setBrightness(100);
  // delay(1000);
  // stripDriver.writePattern(0b01000000);
  // delay(1000);
  // stripDriver.writePattern(0b00100000);
  // delay(1000);
}
