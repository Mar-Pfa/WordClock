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
   Serial No. 1 NodeMcu ESP12E
   Dayana
   HD
   
   Serial No. 2 NodeMcu ESP12E
   Linda Nbg
   HD
   
   Serial No. 3 NodeMcu 1.0 ESP-12E
   SD
   Linda Siegsdorf
   
   Serial No. 4 Wemos D1 mini
   Doris
   HD


FW History
  1002 add working OTA Support
  1003 add Siegsdorf Wifi
  1004 - 1006 add color change mode        
   Firmware Version 1008 changes
   - added "es ist" always on vs only half and full hour
   - added "speechmode" to viertel nach eins vs. viertel zwei
   - fixed "hdmode" storage on website
   Firmware Version 1009 changes
   - bugfixes for 1008
   Firmware Version 1010 changes
   - bugfixes for 1009 - fixed clock display, new stable version for online delivery
   Firmware Version 1011 changes
   - remove ntpclient and switch to time.h to fix Daylight Saving Time behaviour
   Firmware Version 1012 changes
   - try wifi reconnect after 30 seconds if first try was not successfull
   Firmware Versino 1013 changes
   - support for "orig" watch dial 
   Firmware Version 1014 changes
   - support for Arduino OTA 
   - support WifiMultiAp
   - support mDNS responder - open website with devicename.local 
*/

const int FW_VERSION = 1014;
const long utcOffsetInSeconds = 3600;



#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

/* Configuration of NTP */
#define MY_NTP_SERVER "at.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"  
#include <time.h>   
/* Globals */
time_t now;                         // this is the epoch
tm tm;                              // the structure tm holds time information in a more convient way


#include "helpers.h"
#include "config.h"
#include "watch.h"
#include "website.h"
#include <ArduinoOTA.h>

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
  
  configTime(MY_TZ, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!

  //timeClient.begin();

  InitServer();
  
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");  
  
  
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });  
  ArduinoOTA.setHostname(config.DeviceName.c_str());
  ArduinoOTA.begin();
}

void loop() {  
  ArduinoOTA.handle();
  // timeClient.update();
  time(&now);
  localtime_r(&now, &tm);           // update the structure tm with the current time
  
  WatchLoop();
  WifiStatusCheck();
  for (int i=0;i<10;i++)
  {
    WebServerLoop();   
    delay(0);
  }
  
  // every 8 hours -> restart
  if (LoopCounter2 > 2 * 60 * 60 * 8)
  {
      LoopCounter2 = 0;
      ESP.restart();
  }  
}
