#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#define RETRY_TIME 5

#define TYPE_CLIENT 0
#define TYPE_SERVER 1

const uint8_t ledPin = 0;
const uint8_t dataCmdPin = 2;
const uint8_t txPin = 1;
const uint8_t rxPin = 3;

uint32_t conf_baudrate = 9600;
char conf_ssid[32] = "TP-LINK";
char conf_passwd[32] = "12345678";
char conf_targetIP[16] = "192.168.1.100";
uint16_t conf_port = 8266;
uint8_t conf_type = TYPE_CLIENT;

SoftwareSerial mySerial(rxPin, txPin);
WiFiClient myClient;
WiFiServer myServer(conf_port);

void setup()
{
  pinMode(dataCmdPin, INPUT);
  pinMode(ledPin, OUTPUT);

  mySerial.begin(conf_baudrate);
}

void loop()
{
  // put your main code here, to run repeatedly:
}

bool WifiInit()
{
  uint8_t i;
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(conf_ssid, conf_passwd);
  for (i = 0; i < RETRY_TIME; i++)
  {
    delay(1000);
    if (WiFi.status() == WL_CONNECTED)
      break;
  }
  if (WiFi.status() != WL_CONNECTED)
    return false;
  if (conf_type == TYPE_CLIENT)
  {
    myClient.setNoDelay(true);
    myClient.connect(conf_targetIP, conf_port);
    for (i = 0; i < RETRY_TIME; i++)
    {
      delay(1000);
      if (myClient.connected())
        break;
    }
    if (!myClient.connected())
      return false;
  }
  else if (conf_type == TYPE_SERVER)
  {
    myServer.setNoDelay(true);
    myServer.begin(conf_port);
  }
  return true;
}
