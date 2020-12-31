#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#define RETRY_TIME 5

#define NETTYPE_TCPCLIENT 0
#define NETTYPE_TCPSERVER 1
#define NETTYPE_UDP 2

#define DCPIN_DATA HIGH
#define DCPIN_CMD LOW

const uint8_t ledPin = 0;
const uint8_t dataCmdPin = 2; //HIGH is data, LOW is cmd
const uint8_t txPin = 1;
const uint8_t rxPin = 3;

uint32_t conf_baudrate = 9600;
char conf_ssid[32] = "TP-LINK";
char conf_passwd[32] = "12345678";
char conf_targetIP[16] = "192.168.1.100";
uint16_t conf_port = 8266;
uint8_t conf_netType = NETTYPE_TCPCLIENT;

SoftwareSerial mySerial(rxPin, txPin);
WiFiClient myClient;
WiFiServer myServer(conf_port);

String cmdBuffer;

void setup()
{
  pinMode(dataCmdPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  mySerial.begin(conf_baudrate);
}

void loop()
{
  if (digitalRead(dataCmdPin) == DCPIN_DATA)
  {
    if (conf_netType == NETTYPE_TCPCLIENT)
      while (mySerial.available())
        myClient.write(mySerial.read());
    else
      while (mySerial.available())
        myServer.write(mySerial.read());
  }
  else
  {
    if(!mySerial.available())
    return;
      cmdBuffer=mySerial.readString();
  }
}

bool NetInit()
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
  if (conf_netType == NETTYPE_TCPCLIENT)
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
  else if (conf_netType == NETTYPE_TCPSERVER)
  {
    myServer.setNoDelay(true);
    myServer.begin(conf_port);
  }
  return true;
}
