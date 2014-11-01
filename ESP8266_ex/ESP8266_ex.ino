#include "ESP8266.h"
#include "wifiConfig.h" //this is where you need to set your SSID and Password
#define DST_IP "212.71.255.157" //ukhas.net 212.71.255.157
#define WIFI_EN 7 //CH_PD

ESP8266 esp8266 = ESP8266();

boolean ready = false;
void setup()
{
  Serial.begin(9600); //Open serial communications 
  esp8266.initialise(Serial, WIFI_EN); //Pass it over to the ESP8266 class, along with the pin number to enable the module (CH_PD)
  while (!esp8266.resetModule()); //reset module until it is ready
  esp8266.tryConnectWifi(SSID, PASS);//connect to the wifi
  esp8266.singleConnectionMode(); //set the single connection mode
}

void loop()
{
  esp8266.uploadPacket(DST_IP);
  delay(10000);
}

