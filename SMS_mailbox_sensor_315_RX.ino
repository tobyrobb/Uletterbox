/*
My attempt at a SMS connected mailbox sensor

T Robb nov 2014

Radiohead library used for 315 ASK RX code, modified to use NOT pins 11 and 12, but A3,D3

D3 TX, A3 RX

NOTES:

using as letterbox sender 241114
pushbutton on side of device must currently
be pushed to connect to network on power up


//wdt_reset();    //make sure we reset watchdog timer to prevent endless resetting
 
*/

// Includes

#include <SoftwareSerial.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

// Defines

#define speakerPin 5  // The onboard speaker pin

// declare some variables here if you like

char incoming_char=0;
int incomingByte;      // A variable to read incoming mySerial data into
int triggerNum = 0;   //how many letters received
int messageNum = 0;   //how many letters received
unsigned long timeNow = 0;
unsigned long timeBefore = 0;
unsigned long waitReset = 30000000;    //30 000 000(8 hours) time to wait before a reset
unsigned long waitTime = 15000000;    //15 000 000(4 hours)  time to wait between SMS
unsigned long waitSensor = 120000;  //120 000 time to wait for the sensor to stop detecting

RH_ASK driver;
SoftwareSerial SIM900(7, 8);









void setup()
{
  Serial.println(" Beginning Setup ");
  pinMode(speakerPin, OUTPUT);  // If the speaker is fitted.
 
    Serial.begin(19200);	// change to suit bluetooth if connected
    if (!driver.init()){
         Serial.println("init failed");
    }
  Serial.println(" Booting Mobile Phone ");
  SIM900.begin(19200); // for GSM shield
  Serial.println("Waiting 5 seconds to log on to network");

  delay(5000);  // give time to log on to network.
 
  SIM900.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(100);
  SIM900.print("AT+CNMI=2,2,0,0,0\r"); 
  // blurt out contents of new SMS upon receipt to the GSM shield's serial out
  delay(100);
  Serial.println("Setup Complete");
  
 
  tone(speakerPin,1000);  //beep the speaker
  delay(200);        //wait half a sec
  noTone(speakerPin);  //turn speaker off
  tone(speakerPin,500);  //beep the speaker
  delay(200);        //wait half a sec
  noTone(speakerPin);  //turn speaker off
  tone(speakerPin,1000);  //beep the speaker
  delay(200);        //wait half a sec
  noTone(speakerPin);  //turn speaker off

}









void loop()
{
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = 1; //sizeof(buf);  // small packet thanks
    
    timeNow = millis();
    if(messageNum <= 3 && timeNow - timeBefore > waitReset){  //up to 3 pieces of mail and 4 hrs passed
       messageNum = 0;  // reset mail
       }
  // display any text that the GSM shield sends out on the serial monitor
  if(SIM900.available() >0)
  {
    incoming_char=SIM900.read(); //Get the character from the cellular serial port.
    Serial.print(incoming_char); //Print the incoming character to the terminal.
    if(incoming_char== '$'){
      Serial.print("Turning OFF shield");
      delay(3000);
      SIM900power();  // turn off shield
    }
   }
    
  if(messageNum > 3){  //more than 3 pieces of mail
      Serial.print("Something is wrong - too many(");
      Serial.print(messageNum);
      Serial.print(") SMS sent in ");
      Serial.print(waitReset);
      Serial.println(" milliseconds - SHUTTING DOWN");
      SIM900power();  //something wrong
      while(1);
    }
// lets check the buffer for incoming radio packets

    if(driver.recv(buf, &buflen)) // Non-blocking
    {
      Serial.println("Inside receive Loop..");
	int i;
	// Message with a good checksum received, dump it.
	driver.printBuffer("Character is:", buf, buflen);
    Serial.println(buf[i]);
    
     if(buf[i]==49){  //correct character
      Serial.println("Correct character received - MAILBOX OPEN");
      triggerNum++;
      tone(speakerPin,1000);  //beep the speaker
      delay(200);        //wait half a sec
      noTone(speakerPin);  //turn speaker off
      tone(speakerPin,2000);  //beep the speaker
      delay(200);        //wait half a sec
      noTone(speakerPin);  //turn speaker off
      
      // now only send if its OK
      
      if(messageNum < 4 && timeNow - timeBefore > waitTime)  // less than 3 pieces of mail and 4 hrs passed
       {
        Serial.println("Sending message");
        SendTextMessage();
        Serial.println("Message sent");
        Serial.println("Delay for sensor to stop");
        delay(waitSensor);
        timeNow = millis();
          }
       }
  
     if(*buf!=49){
      Serial.println("Incorrect character received..");
      }
  }  
  
  //print some status stuff
  
Serial.print("Time is : ");
Serial.print(timeNow);
Serial.print(" Triggers : ");
Serial.print(triggerNum);
Serial.print(" SMS : ");
Serial.print(messageNum);
  if(messageNum < 4 && timeNow - timeBefore > waitTime){
   Serial.print(" OKAY TO SEND ");
  }
  if(timeNow - timeBefore < waitTime){
   Serial.print(" SEND INHIBIT - TOO SOON ");
  }
  if(messageNum > 3){
   Serial.print(" SEND INHIBIT - TOO MANY SMS SENT ");
  }
  
Serial.println(" Waiting..");
 }










void SendTextMessage()
{
 SIM900.print("AT+CMGF=1\r");    //Because we want to send the SMS in text mode
 delay(100);
 SIM900.println("AT + CMGS = \"+61406039649\"");//send sms message, be careful need to add a country code before the cellphone number
 delay(100);
 SIM900.print("You've got mail! ");//the content of the message
 delay(100);
 SIM900.println((char)26);//the ASCII code of the ctrl+z is 26
 delay(100);
 SIM900.println();
 delay(5000);

 timeBefore = millis();
 messageNum++;

 //PLAY A TUNE
  tone(speakerPin,1000);  //beep the speaker
  delay(150);        //wait half a sec
  noTone(speakerPin);  //turn speaker off
  delay(150);        //wait half a sec
  tone(speakerPin,1000);  //beep the speaker
  delay(150);        //wait half a sec
  noTone(speakerPin);  //turn speaker off
  delay(150);        //wait half a sec
  tone(speakerPin,1000);  //beep the speaker
  delay(150);        //wait half a sec
  noTone(speakerPin);  //turn speaker off
  delay(150);        //wait half a sec

  delay(500);
  
  tone(speakerPin,1000);  //beep the speaker
  delay(500);        //wait half a sec
  noTone(speakerPin);  //turn speaker off
  delay(150);        //wait half a sec
  tone(speakerPin,1000);  //beep the speaker
  delay(500);        //wait half a sec
  noTone(speakerPin);  //turn speaker off
  delay(150);        //wait half a sec

  delay(500);

  tone(speakerPin,1000);  //beep the speaker
  delay(150);        //wait half a sec
  noTone(speakerPin);  //turn speaker off
  delay(150);        //wait half a sec
  tone(speakerPin,1000);  //beep the speaker
  delay(150);        //wait half a sec
  noTone(speakerPin);  //turn speaker off
  delay(150);        //wait half a sec
  tone(speakerPin,1000);  //beep the speaker
  delay(150);        //wait half a sec
  noTone(speakerPin);  //turn speaker off
  delay(150);        //wait half a sec

  delay(1000);

  }

void SIM900power()
// software equivalent of pressing the GSM shield "power" button
{
  digitalWrite(9, HIGH);
  delay(3000);
  digitalWrite(9, LOW);
  delay(7000);
  while(1);
}

