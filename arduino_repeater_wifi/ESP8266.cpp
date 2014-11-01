/*
  ESP8266.cpp - Library for uploading to ukhas.net from ESP8266.
  Created by Chris Stubbs, November 1, 2014.
  Released into the public domain.
*/

#include "Arduino.h"
#include "ESP8266.h"


ESP8266::ESP8266()
{
}

void ESP8266::initialise(HardwareSerial &serial, int pin_en) {
  pinMode(pin_en, OUTPUT);
  digitalWrite(pin_en, HIGH);
  _serial = &serial;
}

boolean ESP8266::connectWifi(String ssid, String pass) {
  delay(1000);  
  _serial->println("AT+CWMODE=1"); //Set the module into mode XYZ
  
  delay(500); // wait for the response from the above command before clearing the buffer
  while(_serial->available()){_serial->read();} //clear the incoming buffer
  
  //Construct the command to connect to AP
  String cmd="AT+CWJAP=\"";
  cmd+=ssid;
  cmd+="\",\"";
  cmd+=pass;
  cmd+="\"";

  _serial->println(cmd); //Send the command to connect to AP
  delay(5000); //Wait until module has returned response to the above command
  
  //Check if module replied "OK" (connection successful)
  if(_serial->find("OK"))
  {
    return true; //its all gravy
  }
  else
  {
    while(_serial->available()){_serial->read();} //clear the incoming buffer
    _serial->println("--Can not connect to the WiFi.--");
    return false;
  }
}

void ESP8266::tryConnectWifi(String ssid, String pass) {
  boolean connected=false;
  for(int i=0;i<5;i++)
  {
    if(connectWifi(ssid,pass))
    {
      connected = true;
      break;
    }
  }
  if (!connected){
    _serial->println("--Failed to connect to AP after 5 attempts--");
    delay(5000);
  }
}

void ESP8266::singleConnectionMode() {
  _serial->println("AT+CIPMUX=0");
}

boolean ESP8266::resetModule() {
    _serial->println("AT+RST");
    while(_serial->available()){_serial->read();} //clear the incoming buffer
    delay(1000);
    if(_serial->find("Ready"))
    {
      return true;
    }
    else
    {
      _serial->println("--Module did not respond.--");
      delay(1000);
      return false;
    }
}

String ESP8266::getIP() { //this is broken, I will fix it when I actually need it...
  delay(3000); // Wait for module to echo above command
  while(_serial->available()){_serial->read();} //clear the incoming buffer
  _serial->println("AT+CIFSR"); //Request for IP
  delay(500); // Wait for module to echo above command
  while(_serial->available()){_serial->read();} //clear the incoming buffer
  delay(2000); // Wait for IP to come in
  String ipString;
  
  //Read reply from module and return
  while (_serial->available()) {
    delay(3);  //delay to allow buffer to fill 
    if (_serial->available() >0) {
       char c = _serial->read();  //gets one byte from serial buffer
       ipString += c; //makes the string readString
    } 
  }
  return ipString;
}


void ESP8266::uploadPacket(String dst_ip, String packet) {
  //Build command to connect to server and send command
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += dst_ip;
  cmd += "\",80";//assuming port 80
  _serial->println(cmd);
  
  while(_serial->available()){_serial->read();} //clear the incoming buffer
  delay(100);
  
  if(_serial->find("Error")) return; //we screwed up
  
  //Build up POST request
  //String postvars = "origin=CS5&data=5aT23[CS1]";
  cmd = "POST /api/upload HTTP/1.0\n";
  cmd += "Host: ukhas.net\n";
  cmd += "Content-Type: application/x-www-form-urlencoded\n";
  cmd += "Content-Length: ";
  cmd += packet.length();
  cmd += "\n";
  cmd += "Connection: close\n\n";
  cmd += packet;
  cmd += "\r\n\r\n";
  
  _serial->print("AT+CIPSEND=");//Send command for POST request
  while(_serial->available()){_serial->read();} //clear the incoming buffer
  _serial->println(cmd.length());//Send length of POST request
  delay(100); //wait for prompt for request
  if(!_serial->find(">")) //Did we find the ">" (prompt for request)?
  {
    _serial->println("AT+CIPCLOSE");
    delay(1000);
    return; //we screwed up
  }
  _serial->print(cmd); //Things look good, send the POST request
  delay(2000);
}

