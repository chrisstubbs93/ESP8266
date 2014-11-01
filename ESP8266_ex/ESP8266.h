/*
  ESP8266.h - Library for uploading to ukhas.net from ESP8266.
  Created by Chris Stubbs, November 1, 2014.
  Released into the public domain.
*/

#ifndef ESP8266_h
#define ESP8266_h

#include "Arduino.h"

class ESP8266
{
  public:
    ESP8266();
    void initialise(HardwareSerial &serial, int pin_en);
    void tryConnectWifi(String ssid, String pass);
    boolean connectWifi(String ssid, String pass);
    boolean resetModule();
    void singleConnectionMode();
    String getIP();
    void uploadPacket(String dst_ip);
  private:
  HardwareSerial* _serial;
  boolean ready;
};

#endif
