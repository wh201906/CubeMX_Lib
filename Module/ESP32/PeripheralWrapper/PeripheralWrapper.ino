#include <HardwareSerial.h>
#include <driver/spi_slave.h>
#include <driver/gpio.h>
#include <Ticker.h>
#include <Arduino.h>

#define WRAPPER_SERIAL_BAUDRATE 2000000
#define WRAPPER_RX_PIN 16
#define WRAPPER_TX_PIN 17

// max freq with GPIO matrix enabled: 7.2MHz
#define WRAPPER_SCLK_PIN 13
#define WRAPPER_MOSI_PIN 12
#define WRAPPER_MISO_PIN 14

HardwareSerial comSerial(1);

spi_bus_config_t buscfg = {
  .mosi_io_num = WRAPPER_MOSI_PIN,
  .miso_io_num = WRAPPER_MISO_PIN,
  .sclk_io_num = WRAPPER_SCLK_PIN,
  .quadwp_io_num = -1,
  .quadhd_io_num = -1,
};

spi_slave_interface_config_t slvcfg = {
  .mode = 0,
  .spics_io_num = -1,
  .queue_size = 1,
  .flags = 0,
  .post_setup_cb = my_post_setup_cb,
  .post_trans_cb = my_post_trans_cb
};

uint8_t syncVariable = 0;
uint8_t asyncVariable = 0;
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
  comSerial.begin(WRAPPER_SERIAL_BAUDRATE, SERIAL_8N1, WRAPPER_RX_PIN, WRAPPER_TX_PIN);
  comTicker.attach_ms(500, comTickerTask);
  sensorTicker.attach_ms(1500, sensorTickerTask);
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (comSerial.available())
  {
    receiveLen = comSerial.readBytes(receiveBuffer, 128);
    Serial.println(receiveLen);
    Serial.write(receiveBuffer, receiveLen);
  }
}
