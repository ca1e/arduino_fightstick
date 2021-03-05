/**************************************************
 * Simple pass-through serial flash programmer 
 * programming the ESP8266 by an Arduino
 * 
 * This resembles the comfortable DTS controlled 
 * programming mode one have with an FTDI or similiar
 * serial connection cable, where no manual reset of
 * the ESP is needed to upload code and run it.
 * Unfortunately there is no RTS/DTS control on the
 * Arduino Serial library, so we solely rely on timing.
 *
 * If the esptool does not wait or retry long enough, 
 * you have to press the reset button
 *
 * Transmissions from the ESP are passed without any
 * modification.
 *
 *
 ***************************************************/

/*
 * connection table:
 * ESP8266  Cactus Micro Rev2
 * GPIO0    A3
 * ENABLE   A2
 * RX       TX
 * TX       RX
 * GND      GND
 */
#define FLASH_MODE
int program_pin = A3;
int enable_pin = A2;

void setup()
{
  Serial1.begin(115200);
  Serial.begin(115200);
  pinMode(program_pin, OUTPUT);

  //delay(5000);
  pinMode(enable_pin, OUTPUT);
  digitalWrite(program_pin, LOW);
  digitalWrite(enable_pin,LOW);
//
//  delay(5 000);
//  digitalWrite(program_pin, HIGH);
//  digitalWrite(enable_pin,HIGH);

  Serial.println("ESP8266 programmer ready.");
}

void loop()
{
  // pass data from ESP to host, if any
  while(Serial1.available())
  {
    Serial.write((uint8_t)Serial1.read());
  }

  // pass data from host to ESP, if any
  if(Serial.available())
  {
    while(Serial.available())
    {
      Serial1.write((uint8_t)Serial.read());
    }
  }
}
