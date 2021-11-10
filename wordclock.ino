/*
 * open issues
 * AdjustTime -> manually -> method disabled at the moment
 * 
 * needed libraries
 * NTPClient
 * AsyncElegantOta
 * 
 * EspAsyncTcp - install manually from https://github.com/me-no-dev/ESPAsyncTCP
 * ESPAsyncWebServer - install manually from https://github.com/me-no-dev/ESPAsyncWebServer
 * 
   Serial No. 1
   Dayana
   HD
   Serial No. 2
   Linda Nbg
   HD
   Serial No. 3
   SD
   Linda Siegsdorf
   Serial No. 4
   Doris
   HD
   Wemos D1 mini
*/

const int FW_VERSION = 1002;
const long utcOffsetInSeconds = 3600;


#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

#include "helpers.h"
#include "config.h"
#include "watch.h"
#include "website.h"

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void setup(){
  EEPROM.begin(512);  
  Serial.begin(115200);

  delay(10);
  Serial.println("Starting ES8266");

  initDisplay();

  if (!ReadConfig())
  {
    setDefaultConfig();
  }
  
  if (config.ssid == "")
  {
    config.ap = true;
    config.ssid = "wordclock";
    config.password = defaultApPassword;
  }


  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");

  strip.begin();
  strip.show();
  digitalWrite(ledPin, HIGH);

  initWifi();
  
  timeClient.begin();

  InitServer();
  
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");  
  
}

void loop() {  
  timeClient.update();

  WatchLoop();
  WebServerLoop();

  delay(10);
  // every 8 hours -> restart
  if (LoopCounter2 > 2 * 60 * 60 * 8)
  {
      LoopCounter2 = 0;
      ESP.restart();
  }  
}
