#include <HardwareSerial.h>
#include <Ticker.h>
#include <Arduino.h>

HardwareSerial comSerial(1);

uint8_t syncVariable=0;
uint8_t asyncVariable=0;
uint8_t receiveBuffer[128];
uint8_t receiveLen;

Ticker comTicker;
Ticker sensorTicker;

void comTickerTask()
{
  comSerial.write(syncVariable);
}

void sensorTickerTask()
{
  syncVariable++;
}


void init()
{
  // baudrate, mode, RxPin, TxPin
  comSerial.begin(2000000,SERIAL_8N1,16,17);
  comTicker.attach_ms(500,comTickerTask);
  sensorTicker.attach_ms(1500,sensorTickerTask);
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(comSerial.available())
  {
    receiveLen=comSerial.readBytes(receiveBuffer,128);
    Serial.println(receiveLen);
    Serial.write(receiveBuffer,receiveLen);
  }
}
