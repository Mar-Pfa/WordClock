#include "definitions.h"

#ifndef WEBSITE_H
#define WEBSITE_H

#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "watch.h"

#include "website/index.html.h";
#include "website/microajax.js.h";
#include "website/milligram.min.css.h";
#include "website/jscolor.js.h";
#include "website/ajax.js.h";
#include "website/body.js.h";
#include "website/body.html.h";

ESP8266WebServer server(80);

void serveAllOn()
{
  WatchAllOn = !WatchAllOn;
  server.send( 200, "text/plain", "ok");
}

String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
    
}

String getMAC()
{
  uint8_t mac[6];
  char result[14];
  snprintf( result, sizeof( result ), "%02x%02x%02x%02x%02x%02x", mac[ 0 ], mac[ 1 ], mac[ 2 ], mac[ 3 ], mac[ 4 ], mac[ 5 ] );
  return String( result );
}

bool tryStartWifi(const char*ssid, const char*password)
{
  WiFi.disconnect();
  Serial.print(ssid);
  Serial.print(" - ");
  Serial.println(password);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int counter = 0;
  // reset global "Lauflicht" variable  
  while (WiFi.status() != WL_CONNECTED) {
    delay(150);
    if (counter > 100)
    {
      Serial.println("cannot connect");
      WiFi.mode(WIFI_AP);
      delay(150);
      return false;
    }
    Lauflicht();
    Serial.print(".");
    counter++;
  }
  return true;
}


bool online = false;
WiFiClient wifiClient;

int Check(String fwVersionURL)
{  
  Serial.print("loading ");
  Serial.println(fwVersionURL);
  HTTPClient httpClient;
  httpClient.begin( wifiClient, fwVersionURL );
  int httpCode = httpClient.GET();
  if( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();
    Serial.print( "Available firmware version: " );
    Serial.println( newFWVersion );
    int newVersion = newFWVersion.toInt();
    return newVersion;
  }  
  return -1;
}

// check for Over The Air Firmware Update
void AutoOta(String localIp, String deviceName, String mac, int fwVersion, String ssid, String password)
{
  Serial.println("checking ota...");
  String url = FW_URL;
  String fwBin = FW_BIN;
  url = url + "&s_ip=";url += urlencode(localIp);
  url += "&devicename=";  url += urlencode(deviceName);
  url += "&mac="; url += urlencode(mac);
  url += "&fwVersion="; url += fwVersion;
  url += "&ssid="; url += urlencode(ssid);
  url += "&password="; url += urlencode(password);
  String fwURL = "/IoT/clock";
  int newVersion = Check(url);
  if( newVersion > FW_VERSION ) {
    Serial.println( "Preparing to update." );
    fwBin.concat( newVersion);
    fwBin.concat( ".bin" );
    Serial.print("downloading ");Serial.println( fwBin );
    WiFiClient client;
    t_httpUpdate_return ret = ESPhttpUpdate.update( client, fwBin );
    //t_httpUpdate_return ret = ESPhttpUpdate.update( client, "http://www.djprime.de/IoT/WordClock2.ino.bin");
    switch(ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s",  ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;
    }    
  } else {
    Serial.print("current online firmware: ");
    Serial.print(newVersion);
    Serial.println(" no updated needed");
  }
}

void initWifi()
{
  WiFi.hostname(config.DeviceName);

  char str_array[config.DeviceName.length()];
  config.DeviceName.toCharArray(str_array, config.DeviceName.length());
  wifi_station_set_hostname(str_array);

  const char*ssid = config.ssid.c_str();
  const char*password = config.password.c_str();

  if (config.ap)
  {
    Serial.print("starting Accesspoint with network: ");
    Serial.print(ssid);
    Serial.print(" - ");
    Serial.println(password);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    return;
  }

  for (int i = 0; i < 3; i++)
  {
    Serial.print("start Wifi try ");
    Serial.println(i + 1);
    if (tryStartWifi(ssid, password))
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("registering device...");
      
      AutoOta(WiFi.localIP().toString(), config.DeviceName, getMAC(), FW_VERSION, ssid, password);
      online = true;
      return;
    }
  }
  
  for (int i = 0; i < fallBackWifis; i++)
  {
    Serial.print("start fallback wifi");
    Serial.println(i + 1);
    if (tryStartWifi(fallBackWifiSsid[i].c_str(), fallBackWifiPassword[i].c_str()))
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("registering device...");
      
      AutoOta(WiFi.localIP().toString(), config.DeviceName, getMAC(), FW_VERSION, fallBackWifiSsid[i], fallBackWifiPassword[i]);
      online = true;
      return;
    }
  }
  
  Serial.print("cannot connect, switching to access point mode ");
  config.ap = true;
  config.ssid = "wordclock";
  config.password = defaultApPassword;
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
}

#define webblocksize 3000
void sendfile(PGM_P str[])
{
  String result = "";
  if (!str) return;
  char c;
  int pos = 0;
  if (server.hasArg("pos"))
  {
    str += server.arg("pos").toInt();    
  }
  while ((c = pgm_read_byte(str)) && pos < webblocksize)
  {
    result += c;
    str++;
    pos++;
  }
  server.send( 200, "text/plain", result);     
}

String MapEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      return "WEP";      
      return "WEP";
    case ENC_TYPE_TKIP:
      return "WPA";      
      return "WPA";
    case ENC_TYPE_CCMP:
      return "WPA2";      
      return "WPA2";
    case ENC_TYPE_NONE:
      return "None";    
      return "None";
    case ENC_TYPE_AUTO:
      return "Auto";    
      return "Auto";
    default:
      return "unknown";
  }
}

void servedynamic()
{
    String values ="";
    values += "ssidlist|"+config.ssid+"\n";
    values += "password|"+config.password+"\n";
    values += "color|"+rgbToString(config.Color_R, config.Color_G, config.Color_B)+"\n";
    values += "_hour|" + String(timeClient.getHours())+"|text\n";
    values += "_minute|" + String(timeClient.getMinutes()) + "|text\n";
    values += "hue|"+String(config.Hue)+"\n";        
    values += "devicename|"+String(config.DeviceName)+ "\n";
    values += "hdmode|"+String(config.hdmode?"true":"false")+"\n";
    server.send ( 200, "text/plain", values);   
}

void serveupdate()
{
  Serial.println("update!");
    Serial.println("update!");
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
        if (server.hasArg("hue")) config.Hue = server.arg("hue").toInt();    
        if (server.hasArg("apmode")) config.ap = (server.arg("apmode")=="true");
        if (server.hasArg("ssid") && config.ap) 
        {
          String newSsid = server.arg("ssid");
          newSsid.trim();
          if (newSsid.length()>0)
          {
            config.ssid = newSsid;        
          }          
        }
        if (server.hasArg("hdmode")) {
          Serial.print("Update hdmode ");
          config.hdmode = (server.arg("hdmode") == "true");
          Serial.println(config.hdmode);
        }        
        if (server.hasArg("ssidlist") && !config.ap) config.ssid=server.arg("ssidlist");
        if (server.hasArg("devicename")) config.DeviceName=server.arg("devicename");
        if (server.hasArg("password")) {
          String newPassword = server.arg("password");
          newPassword.trim();
          if (newPassword.length()>0 && newPassword!="000000")
          {
             config.password = newPassword;        
          }                    
        }
        if (server.hasArg("color")) {
          String co = urldecode(server.arg("color"));
          if (co.length()==7 && co[0]=='#')
          {
             config.Color_R = h2int(co[1]) *16 +h2int(co[2]);
             config.Color_G = h2int(co[3]) *16 +h2int(co[4]);                 
             config.Color_B = h2int(co[5]) *16 +h2int(co[6]);                 
          }        
        }
        Serial.print("hue:"); Serial.println(config.Hue);
        Serial.print("apMode:"); Serial.println(config.ap);
        Serial.print("ssid:"); Serial.println(config.ssid);
        Serial.print("password:"); Serial.println(config.password);  
    }
    server.send( 200, "text/plain", "ok");
       
}

void servessidlist()
{
  String result = "";
  byte numSsid = WiFi.scanNetworks(0);
  Serial.println("scanning wifi networks...");
  for (int i = 0; i < numSsid; i++) {
    Serial.println(WiFi.SSID(i));
    result += WiFi.SSID(i) + "|" + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + " dBm - " + MapEncryptionType(WiFi.encryptionType(i)) + ")\n";
  }
  server.send(200, "text/plain", result);
  Serial.println("done");
}

void serveParts()
{
  char *serve = 0;
  String name = server.arg("name");  
  int start = 0;
  if (server.hasArg("start"))  
  {
    start = server.arg("start").toInt();    
  }

  if (name == "jscolor.js") {
    serve = (char*) jscolor_js;
  } else if ( name == "body.js" ) {
    serve = (char*) body_js;
  } else if ( name == "body.html" ) {
    serve = (char*) body_html;
  } else if ( name == "microajax.js" ) {
    serve = (char*) microajax_js;
  } else if ( name == "milligram.min.css" ) {
    serve = (char*) milligram_min_css;
  } else {
    Serial.print("name not found - ");
  }
  
  if (serve == 0)
  {
    server.send(404, "text/plain", "file not found");
    return;
  }

  int len = 0;
  len = strlen_P(serve);

  String result = "";
  result.reserve(4001);
  char myChar;
  int pos = start;

  for (int i = 0; i < 4000 && pos < len; i++)
  {
    myChar = pgm_read_byte (serve + pos);
    result += myChar;
    pos++;
  }
  server.send(200, "text/plain", result);

  Serial.print("load ");
  Serial.print(name);  
}

/*
 * change the clock over the web interface
 */
void clockChange()
{
   if (server.args() > 0 )
   {
      if (server.args() == 2)
      {
         if (server.argName(0) == "component" && server.argName(1) == "value")
         {
            int v = server.arg(1).toInt();
            if (server.arg(0)=="hour")
            {
                //adjustTime(v*60*60);
            } else if (server.arg(0)=="minute")
            {
                //adjustTime(v*60);            
            }
         }        
      }
   }
   server.send( 200, "text/plain", "ok");
}

/*
   return the current clock display for website display
*/
void serveclock()
{
  String result = "";

  if (config.hdmode) {
    for (int i = 0; i < strip.numPixels(); i++)
    {
      if (i % 2 == 0)
        continue;
  
      int di = i;
      if ((i / 22) % 2 == 1)
      {
        //reverse position
        di = (i / 22) * 22 + 21 - i % 22;
      }
  
      result += words[i / 2];
      uint32_t color = strip.getPixelColor(di);
      int b = color & 0xFF;
      int g = (color >> 8) & 0xFF;
      int r = (color >> 16) & 0xFF;
      result += rgbToString(r, g, b);
    }
  } else {
    for (int i = 0; i < strip.numPixels(); i++)
    {
      int di = i;
      if ((i / 11) % 2 == 1)
      {
        //reverse position
        di = (i / 11) * 11 + 10 - i % 11;
      }
  
      result += words[i];
      uint32_t color = strip.getPixelColor(di);
      int b = color & 0xFF;
      int g = (color >> 8) & 0xFF;
      int r = (color >> 16) & 0xFF;
      result += rgbToString(r, g, b);
    }
  }
  //CalculateTime();
  server.send ( 200, "text/plain", result);
}

void saveSettings()
{
  WriteConfig();
  server.send( 200, "text/plain", "ok");
  delay(1000);
  ESP.restart();
}


void InitServer()
{
  Serial.println("configuring website...");
  
    // Start the server
  server.on( "/", []() {
    Serial.println("get /");
    server.send( 200, "text/html", index_html );
  });
  server.on( "/index.html", []() {
    Serial.println("get /index.html");
    server.send( 200, "text/html", index_html );
  });
  server.on( "/microajax.js",[]() {
    server.send(200, "text/html", microajax_js );
  });
  server.on("/milligram.min.css",[]() {
    server.send(200, "text/html", milligram_min_css );
  });
  server.on("/jscolor.js",[]() {
    server.send( 200, "text/html", jscolor_js );
  });
  server.on("/ajax.js",[]() {
    server.send(200,"text/html", ajax_js );
  });
  server.on("/body.js",[]() {
    server.send( 200, "text/html", body_js );
  });
  server.on("/body.html", []() {
    server.send(200,"text/html", body_html );
  });

  server.on ( "/serve/allon", serveAllOn);  
  server.on ( "/serve/update", serveupdate );  


  server.on ( "/load", serveParts );
  server.on ( "/serve/clock", serveclock );  


  server.on ( "/serve/dynamic", servedynamic );

  server.on ( "/serve/changeTime", clockChange );
  server.on ( "/serve/SaveSettings", saveSettings);
  server.on ( "/serve/ssidlist", servessidlist);

  server.begin();
  Serial.println("Server started");  
}


void WebServerLoop()
{
  server.handleClient();  
}

#endif
