/*

T Robb nov 2014

Radiohead library used for 315 ASK TX code, modified to use NOT pins 11 and 12, but A3,D3

D3 TX, A3 RX

NOTES:

using as letterbox sender 241114

*/

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver;

#define irPin 10


void setup()
{
    Serial.begin(9600);	  // Debugging only
    if (!driver.init()){
         Serial.println("init failed");
    }
    pinMode(irPin, INPUT);
    digitalWrite(irPin, LOW);
    Serial.println("Setup Complete");
}

void loop()
{
  while(digitalRead(irPin)){
    Serial.println("Received signal ");
    const char *msg = "1";
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(200);
  }
}
