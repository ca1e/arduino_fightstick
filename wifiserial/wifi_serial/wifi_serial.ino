#include <EEPROM.h>

#include "wifi.h"

/*
  |--------------|-------|---------------|--|--|--|--|--|
  ^              ^       ^               ^     ^
  Sketch    OTA update   File system   EEPROM  WiFi config (SDK)
*/

#define LED_BUILTIN 2       //onboard LED
const unsigned int BAUD_RATE      = 9600; //baud rate

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // turn the LED on

  smartConfig();

  digitalWrite(LED_BUILTIN, LOW);
  server.begin();
  server.setNoDelay(true);
}

void reset_esp()
{
  ESP.eraseConfig();
  delay(1000);
  //ESP.reset(); // not recommended
  ESP.restart();
}

void loop()
{
  wifi_loop();
}
