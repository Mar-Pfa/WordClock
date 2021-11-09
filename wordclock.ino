/*
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

const int FW_VERSION = 1001;

#include <ESP8266WiFi.h>

#include <NTPClient.h>
#include "Time.h"
#include "Timezone.h" 
#include <coredecls.h>                  // settimeofday_cb()
//#include "NtpClientLib.h"
//#include <ESP8266WebServer.h>
#include "ESPAsyncWebServer.h"
#include "ESPAsyncTCP.h"
#include "AsyncElegantOTA.h"
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "helpers.h"

#include "index.html.h";
#include "microajax.js.h";
#include "milligram.min.css.h";
#include "jscolor.js.h";
#include "ajax.js.h";
#include "body.js.h";
#include "body.html.h";

//#define startscreen "Dayana"

#define startscreen "Linda"
#define displaySize 114

char *letters = "abcdefghijklmnopqrstuvwxyz0123456789";

const char* baum  = "     y          g        y g y       ggg      y ggg y     ggggg    y ggggg y   ggggggg  y ggggggg y ggggggggg ....";
const char *hiday = "                        W           W WW        W           W           W           W                             ";
const char *heart = "  rrr rrr   rrrrrrrrr rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr rrrrrrrrr   rrrrrrr     rrrrr       rrr         r     rrrr";
const char* words = "ESKISTMFUNFZEHNZWANZIGAPDJVIERTELVORAQHINACHHALBYELFUNFEINSWAXZWEIDREIAPNVIERSECHSGLACHTSIEBENZWOLFZEHNEUNTUHR    ";
//                   01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
//                   00000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000
const byte word_es[2] = {0, 1};
const byte word_ist[2] = {3, 5};
const byte word_uhr[2] = {107, 109};
const byte word_m_fuenf[2] = {7, 10};
const byte word_m_zehn[2] = {11, 14};
const byte word_m_viertel[2] = {26, 32};
const byte word_m_zwanzig[2] = {15, 21};
const byte word_m_vor[2] = {33, 35};
const byte word_m_nach[2] = {40, 43};
const byte word_m_halb[2] = {44, 47};

const byte word_h_ein[2] = {55, 57};
const byte word_h_eins[2] = {55, 58};
const byte word_h_zwei[2] = {62, 65};
const byte word_h_drei[2] = {66, 69};
const byte word_h_vier[2] = {73, 76};
const byte word_h_fuenf[2] = {51, 54};
const byte word_h_sechs[2] = {77, 81};
const byte word_h_sieben[2] = {88, 93};
const byte word_h_acht[2] = {84, 87};
const byte word_h_neun[2] = {102, 105};
const byte word_h_zehn[2] = {99, 102};
const byte word_h_elf[2] = {49, 51};
const byte word_h_zwoelf[2] = {94, 98};

//const byte* word_hours[12] = {word_h_zwoelf, word_h_eins, word_h_zwei, word_h_drei, word_h_vier, word_h_fuenf, word_h_sechs, word_h_sieben, word_h_acht, word_h_neun, word_h_zehn, word_h_elf};
const byte word_hours[12][2] = {{94, 98}, {55, 58}, {62, 65}, {66, 69}, {73, 76}, {51, 54}, {77, 81}, {88, 93}, {84, 87}, {102, 105}, {99, 102}, {49, 51}};

// lauflicht variable
int run = 0;


char displayarray[displaySize + 1];
void initDisplay()
{
  // set every entry to "space"
  for (int i = 0; i < displaySize; i++)
  {
    displayarray[i] = 32;
  }
  // and the last one to zero as string terminator
  displayarray[displaySize] = 0;
}

void updateDisplay(byte from, byte to)
{
  for (int i = from; i <= to; i++)
  {
    displayarray[i] = words[i];
  }
}

#define StripPin D5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(displaySize * 2, StripPin, NEO_GRB + NEO_KHZ800);


#define ledPin D4

//WiFiServer server(80); //Initialize the server on Port 80
AsyncWebServer server(80);


#define defaultApPassword "00000000" 
void setDefaultConfig()
{
  // DEFAULT CONFIG
  config.ssid = "workclock";
  config.password = defaultApPassword;
  config.dhcp = true;
  config.ap = true;
  config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 2; config.IP[3] = 252;
  config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
  config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 2; config.Gateway[3] = 1;
  config.ntpServerName = "0.de.pool.ntp.org";
  config.Update_Time_Via_NTP_Every =  0;
  config.timezone = +1;
  config.daylight = false;
  config.hdmode = true;
  config.DeviceName = "clock_";
  for (int i = 0; i < 5; i++)
  {
    byte randomValue = random(0, 26);
    char letter = randomValue + 'a';
    config.DeviceName = config.DeviceName + letter;
  }

  config.Color_R = 255;
  config.Color_G = 255;
  config.Color_B = 255;
  config.Hue = 255;
  config.Dynamic = 0;
}

bool tryStartWifi(const char*ssid, const char*password)
{
  WiFi.disconnect();
  Serial.print(ssid);
  Serial.println(password);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int counter = 0;
  // reset global "Lauflicht" variable
  run = 0;
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
    run++;
    Serial.print(".");
    counter++;
  }
  return true;
}


bool online = false;

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

  for (int i = 0; i < 5; i++)
  {
    Serial.print("start Wifi try ");
    Serial.println(i + 1);
    if (tryStartWifi(ssid, password))
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("registering device...");
      AutoOta(WiFi.localIP().toString(), config.DeviceName, getMAC(), FW_VERSION);
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
void sendfile(AsyncWebServerRequest *request, PGM_P str[])
{
  String result = "";
  if (!str) return;
  char c;
  int pos = 0;
  if (request->hasParam("pos"))
  {
    str += request->getParam("pos")->value().toInt();
  }
  while ((c = pgm_read_byte(str)) && pos < webblocksize)
  {
    result += c;
    str++;
    pos++;
  }
  request->send(200, "text/plain", result);
  /*
    char *buffer = new char[1000]();
    strncpy((char*)was, buffer, 1000);
    //strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i])));
    server.send(200,"text/html", buffer);
  */
}


#define TZ              1       // (utc+) TZ in hours
#define DST_MN          60      // use 60mn for summer time in some countries
#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

timeval cbtime;      // time set in callback
bool cbtime_set = false;

void time_is_set(void) {
  gettimeofday(&cbtime, NULL);
  cbtime_set = true;
  Serial.println("------------------ settimeofday() was called ------------------");
}


void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  delay(10);
  Serial.println("Starting ES8266");

  initDisplay();

  if (!ReadConfig())
  {
    setDefaultConfig();
  }

/*
  if (config.hdmode) {
     strip = Adafruit_NeoPixel(displaySize * 2, StripPin, NEO_GRB + NEO_KHZ800);    
  } else {
     strip = Adafruit_NeoPixel(displaySize, StripPin, NEO_GRB + NEO_KHZ800);    
  }
*/

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

  // Start the server
  server.on ( "/", [](AsyncWebServerRequest * request) {
    request->send ( 200, "text/html", index_html );
  } );

  server.on ( "/index.html", [](AsyncWebServerRequest * request) {
    request->send ( 200, "text/html", index_html );
  } );
  server.on ( "/microajax.js", [](AsyncWebServerRequest * request) {
    request->send ( 200, "text/html", microajax_js );
  } );
  server.on ( "/milligram.min.css", [](AsyncWebServerRequest * request) {
    request->send ( 200, "text/html", milligram_min_css );
  } );
  server.on ( "/jscolor.js", [](AsyncWebServerRequest * request) {
    request->send ( 200, "text/html", jscolor_js );
  } );
  server.on ( "/ajax.js", [](AsyncWebServerRequest * request) {
    request->send ( 200, "text/html", ajax_js );
  } );
  server.on ( "/body.js", [](AsyncWebServerRequest * request) {
    request->send ( 200, "text/html", body_js );
  } );
  server.on ( "/body.html", [](AsyncWebServerRequest * request) {
    request->send ( 200, "text/html", body_html );
  } );

  server.on ( "/load", serveParts );

  server.on ( "/serve/clock", serveclock );
  server.on ( "/serve/dynamic", servedynamic );
  server.on ( "/serve/update", serveupdate );
  server.on ( "/serve/changeTime", clockChange );
  server.on ( "/serve/SaveSettings", saveSettings);
  server.on ( "/serve/ssidlist", servessidlist);
  server.on ( "/serve/allon", serveAllOn);

  AsyncElegantOTA.begin(&server);

  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

/*
  NTP.onNTPSyncEvent([](NTPSyncEvent_t error) {
    if (error) {
      Serial.print("Time Sync error: ");
      if (error == noResponse)
        Serial.println("NTP server not reachable");
      else if (error == invalidAddress)
        Serial.println("Invalid NTP server address");
    }
    else {
      Serial.print("Got NTP time: ");
      Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
    }
  });
  if (!NTP.setInterval(5, 60)) {
    Serial.println("Interval Setting not successfull");
  }
  NTP.stop();
  NTP.begin("pool.ntp.org", 1, false);
  */
  /* Configuration of NTP */


  /*
 #define MY_NTP_SERVER "pool.ntp.org"           
 #define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03" 
 configTime(MY_TZ, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!
*/

/*
  time_t rtc = 1510592825;  // 1510592825 = Monday 13 November 2017 17:07:05 UTC
  timeval tv = { rtc, 0 };
  timezone tz = { TZ_MN + DST_MN, 0 };
  settimeofday(&tv, &tz);
  
  settimeofday_cb(time_is_set);
  configTime(TZ_SEC, DST_SEC, "pool.ntp.org");
*/

}


void TimetoConsole()
{
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.println(second());
}

/*
   calculate the correct time
*/
void CalculateTime()
{
  initDisplay();
  int h = hour();
  if (h > 11) h -= 12;
  int m = minute();

  updateDisplay(word_es[0], word_es[1]);
  updateDisplay(word_ist[0], word_ist[1]);

  int hadd = h + 1;
  if (hadd == 12)
    hadd = 0;

  if (m < 5)
  {
    if (h == 1)
    {
      updateDisplay(word_h_ein[0], word_h_ein[1]);
    }
    else
    {
      updateDisplay(word_hours[h][0], word_hours[h][1]);
    }
    updateDisplay(word_uhr[0], word_uhr[1]);
  } else if (m < 10) {
    updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_hours[h][0], word_hours[h][1]);
  } else if (m < 15) {
    updateDisplay(word_m_zehn[0], word_m_zehn[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_hours[h][0], word_hours[h][1]);
  } else if (m < 20) {
    updateDisplay(word_m_viertel[0], word_m_viertel[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_hours[h][0], word_hours[h][1]);
  } else if (m < 25) {
    updateDisplay(word_m_zwanzig[0], word_m_zwanzig[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_hours[h][0], word_hours[h][1]);
  } else if (m < 30) {
    updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
    updateDisplay(word_m_vor[0], word_m_vor[1]);
    updateDisplay(word_m_halb[0], word_m_halb[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else if (m < 35) {
    updateDisplay(word_m_halb[0], word_m_halb[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else if (m < 40) {
    updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_m_halb[0], word_m_halb[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else if (m < 45) {
    updateDisplay(word_m_zehn[0], word_m_zehn[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_m_halb[0], word_m_halb[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else if (m < 50) {
    updateDisplay(word_m_viertel[0], word_m_viertel[1]);
    updateDisplay(word_m_vor[0], word_m_vor[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else if (m < 55) {
    updateDisplay(word_m_zehn[0], word_m_zehn[1]);
    updateDisplay(word_m_vor[0], word_m_vor[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else {
    updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
    updateDisplay(word_m_vor[0], word_m_vor[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  }

  int l;
  for (l = 0; l < m % 5; l ++)
    //for (l=0;l<4; l ++)
  {
    displayarray[l + 110] = '.';
  }
  //Serial.println(displayarray);
}

WiFiUDP ntpUDP;
int GTMOffset = 1;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", GTMOffset*60*60, 60*60*1000);
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
Timezone CE(CEST, CET);
 
void serveParts(AsyncWebServerRequest *request)
{
  char *serve = 0;
  String name = request->getParam("name")->value();
  int start = 0;
  if (request->hasParam("start"))
  {
    start = request->getParam("start")->value().toInt();
  }

  if (name == "jscolor.js") {
    serve = (char*)jscolor_js;
  } else if ( name == "body.js" ) {
    serve = (char*)body_js;
  } else if ( name == "body.html" ) {
    serve = (char*)body_html;
  } else if ( name == "microajax.js" ) {
    serve = (char*)microajax_js;
  } else if ( name == "milligram.min.css" ) {
    serve = (char*) milligram_min_css;
  } else {
    Serial.print("name not found - ");
  }

  if (serve == 0)
  {
    request->send(404, "text/plain", "file not found");
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
  request->send(200, "text/plain", result);

  Serial.print("load ");
  Serial.print(name);
  Serial.print(" from ");
  Serial.print(start);
  Serial.print(" length ");
  Serial.println(result.length());
  //Serial.println(result);
}

/*
   return the current clock display for website display
*/
void serveclock(AsyncWebServerRequest *request)
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
  request->send ( 200, "text/plain", result);
}


long envhue;
/*
   return the dynamic values for the website
*/
void servedynamic(AsyncWebServerRequest *request)
{
  String values = "";
  values += "ssid|" + config.ssid + "\n";
  values += "password|" + config.password + "\n";
  values += "color|" + rgbToString(config.Color_R, config.Color_G, config.Color_B) + "\n";
  values += "_hour|" + String(hour()) + "|text\n";
  values += "_minute|" + String(minute()) + "|text\n";
  values += "hue|" + String(config.Hue) + "\n";
  values += "dynamic|" + String(config.Dynamic) + "\n";
  values += "envhue|" + String((envhue * 100) >> 10 ) + "%|text\n";
  values += "devicename|" + String(config.DeviceName) + "\n";
  values += "hdmode|"+ String(config.hdmode)+"\n";
  request->send ( 200, "text/plain", values);
}

String MapEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      return "WEP";
    case ENC_TYPE_TKIP:
      return "WPA";
    case ENC_TYPE_CCMP:
      return "WPA2";
    case ENC_TYPE_NONE:
      return "None";
    case ENC_TYPE_AUTO:
      return "Auto";
    default:
      return "unknown";
  }
}

bool allOn = false;
void serveAllOn(AsyncWebServerRequest *request)
{
  allOn = !allOn;
  request->send( 200, "text/plain", "ok");
}

void servessidlist(AsyncWebServerRequest *request)
{
  /*
  String result = "";
  byte numSsid = WiFi.scanNetworks(0);
  Serial.println("scanning wifi networks...");
  for (int i = 0; i < numSsid; i++) {
    Serial.println(WiFi.SSID(i));
    result += WiFi.SSID(i) + "|" + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + " dBm - " + MapEncryptionType(WiFi.encryptionType(i)) + ")\n";
  }
  Serial.println("done");
  request->send(200, "text/plain", result);
  */
  String result = "";
  int n = WiFi.scanComplete();
  if(n == -2){
    WiFi.scanNetworks(true);
  } else if(n) {
    for (int i = 0; i < n; ++i){
       Serial.println(WiFi.SSID(i));
       result += WiFi.SSID(i) + "|" + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + " dBm - " + MapEncryptionType(WiFi.encryptionType(i)) + ")\n";
    }
    WiFi.scanDelete();
    if(WiFi.scanComplete() == -2){
      WiFi.scanNetworks(true);
    }
  }
  Serial.println("done");
  request->send(200, "text/plain", result);  
}

/*
   change the clock over the web interface
*/
void clockChange(AsyncWebServerRequest *request)
{
  if (request->params() > 0 )
  {
    if (request->params() == 2)
    {
      if (request->getParam(0)->name() == "component" && request->getParam(1)->value() == "value")
      {
        int v = request->getParam(1)->value().toInt();
        /*            int v = 1;
                    if (server.arg(1)=="-1")
                    {
                      v=-1;
                    }
        */
        if (request->getParam(0)->value() == "hour")
        {
          adjustTime(v * 60 * 60);
        } else if (request->getParam(0)->value() == "minute")
        {
          adjustTime(v * 60);
        }
      }
    }
  }
  request->send( 200, "text/plain", "ok");
}

void saveSettings(AsyncWebServerRequest *request)
{
  WriteConfig();
  request->send( 200, "text/plain", "ok");
  delay(1000);
  ESP.restart();
}
/*

*/
void serveupdate(AsyncWebServerRequest *request)
{
  Serial.println("update!");
  if (request->params() > 0 )  // Are there any POST/GET Fields ?
  {    
    if (request->hasParam("hue")) {
      Serial.print("Update Hue ");
      config.Hue = request->getParam("hue")->value().toInt();
      Serial.println(config.Hue);
    }
    if (request->hasParam("dynamic")) config.Dynamic = request->getParam("dynamic")->value().toInt();
    if (request->hasParam("hdmode")) {
      Serial.print("Update hdmode ");
      config.hdmode = (request->getParam("hdmode")->value() == "true");
      Serial.println(config.hdmode);
    }
    if (request->hasParam("apmode")) config.ap = (request->getParam("apmode")->value() == "true");
    if (request->hasParam("ssid") && config.ap) config.ssid = request->getParam("ssid")->value();
    if (request->hasParam("ssidlist") && !config.ap) {
      String newSsid = request->getParam("ssidlist")->value();
      newSsid.trim();
      if (newSsid.length()>0)
      {
        config.ssid = newSsid;        
      }
    }
    if (request->hasParam("devicename")) config.DeviceName = request->getParam("devicename")->value();
    if (request->hasParam("password")) {
      String newPassword = request->getParam("password")->value();
      newPassword.trim();
      if (newPassword.length()>0 && newPassword!="000000")
      {
         config.password = newPassword;        
      }
    }
    if (request->hasParam("color")) {
      String co = urldecode(request->getParam("color")->value());
      if (co.length() == 7 && co[0] == '#')
      {
        config.Color_R = h2int(co[1]) * 16 + h2int(co[2]);
        config.Color_G = h2int(co[3]) * 16 + h2int(co[4]);
        config.Color_B = h2int(co[5]) * 16 + h2int(co[6]);
      }
    }
    Serial.print("hue:"); Serial.println(config.Hue);
    Serial.print("dynamic:"); Serial.println(config.Dynamic);
    Serial.print("apMode:"); Serial.println(config.ap);
    Serial.print("ssid:"); Serial.println(config.ssid);
    Serial.print("password:"); Serial.println(config.password);
  }
  request->send( 200, "text/plain", "ok");
}


/*
   small helper function to test the complete strip
*/

void Lauflicht()
{
  int run2 = run / 10;
  for (int i = 0; i < strip.numPixels(); i++)
  {
    uint32_t co = 0;
    if (run2 % strip.numPixels() >= i - 2 && run2 % strip.numPixels() <= i + 2)
    {
      co = strip.Color(64, 64, 64);
    }
    strip.setPixelColor(i, co);
  }
  strip.show();
}

void Lauflicht2()
{
  for (int i = 0; i < strip.numPixels(); i++)
  {
    uint32_t co = 0;
    if (run % strip.numPixels() == i)
    {
      co = strip.Color((255 - run % 255) / 2, (255 - run % 255) / 2, (run % 255) / 2);
    }
    if ((run + 1) % strip.numPixels() == i || (run - 1) % strip.numPixels() == i)
    {
      co = strip.Color((255 - run % 255) / 8, (255 - run % 255) / 8, (run % 255) / 8);
    }
    strip.setPixelColor(i, co);
  }
  strip.show();
}

void ShowAllOn()
{
  // calculate dynamic lightning part regarding environmental hue and config settings
  long dyn = config.Dynamic;
  dyn = 255 - dyn; //((long) dyn) * ((long)envhue) >> 10 + 255-dyn;
  long etmp = envhue;
  etmp = (etmp * ((long)config.Dynamic)) >> 10;
  dyn += etmp;
  // calculate base hue regarding config and dynamic lightning
  long h = (((long) config.Hue) * dyn) >> 8;
  long h2 = h >> 1;

  Serial.print("base hue: ");
  Serial.println(h);
  Serial.println(dyn);
  Serial.println(config.Hue);
  Serial.println(config.Dynamic);


  int cr = ((int) config.Color_R * (int) h) >> 8;
  int cg = ((int) config.Color_G * (int) h) >> 8;
  int cb = ((int) config.Color_B * (int) h) >> 8;
  uint32_t coHigh = strip.Color(cr, cg, cb);

  uint32_t coRed = strip.Color(h, 0, 0);
  uint32_t coGreen = strip.Color(0, h, 0);
  uint32_t coYellow = strip.Color(h, h, 0);

  for (int i = 0; i < displaySize; i++)
  {
    if (config.hdmode)
    {
      strip.setPixelColor(i * 2, coHigh);
      strip.setPixelColor(i * 2 + 1, coHigh);      
    } else {
      strip.setPixelColor(i, coHigh);      
    }
  }
  displayarray[displaySize] = 0;
  strip.show();
}

/*
   Display the DisplayArray to the Matrix LED-Display
*/
void ShowTime()
{
  // calculate dynamic lightning part regarding environmental hue and config settings
  long dyn = config.Dynamic;
  dyn = 255 - dyn; //((long) dyn) * ((long)envhue) >> 10 + 255-dyn;
  long etmp = envhue;
  etmp = (etmp * ((long)config.Dynamic)) >> 10;
  dyn += etmp;
  // calculate base hue regarding config and dynamic lightning
  long h = (((long) config.Hue) * dyn) >> 8;
  long h2 = h >> 1;

  Serial.print("base hue: ");
  Serial.println(h);
  Serial.println(dyn);
  Serial.println(config.Hue);
  Serial.println(config.Dynamic);

  int cr = ((int) config.Color_R * (int) h) >> 8;
  int cg = ((int) config.Color_G * (int) h) >> 8;
  int cb = ((int) config.Color_B * (int) h) >> 8;
  uint32_t coHigh = strip.Color(cr, cg, cb);

  uint32_t coRed = strip.Color(h, 0, 0);
  uint32_t coGreen = strip.Color(0, h, 0);
  uint32_t coYellow = strip.Color(h, h, 0);

  char redChar  = 'r';
  char greenChar = 'g';
  char yellowChar = 'y';

  for (int i = 0; i < displaySize; i++)
  {
    int di = i;
    if ((i / 11) % 2 == 1)
    {
      //reverse position
      di = (i / 11) * 11 + 10 - i % 11;
    }
    uint32_t co = 0;
    if (displayarray[di] != 32) {
      if (displayarray[di] == redChar) {
        co = coRed;
      } else if (displayarray[di] == greenChar) {
        co = coGreen;
      } else if (displayarray[di] == yellowChar) {
        co = coYellow;
      } else {
        co = coHigh;
      }
    }
    if (config.hdmode)
    {
      strip.setPixelColor(i * 2, co);
      strip.setPixelColor(i * 2 + 1, co);
    } else {
      strip.setPixelColor(i, co);
    }
  }
  displayarray[displaySize] = 0;
  strip.show();
}


int run2 = 0;

void loop() {
  
  if ( (run < 10 || (run < 100 && run % 10 == 0) || (run < 1000 && run % 100 == 0)) && online && year() < 2000)
  {
    setSyncInterval(0);
//    NTP.setTimeZone(1);    
    now();
    setSyncInterval(60);
  }


  run++;
  if (run < 200)
  {
    Lauflicht();
  }
  else if (run % 40 == 0)
  {
    run2++;

    if (run2 % 16 == 1) TimetoConsole();

    if (startscreen == "Dayana")
    {
      if (run2 < 10)
      {
        for (int i = 0; i < displaySize; i++)
          displayarray[i] = hiday[i];
      } else if (run2 < 20)
      {
        for (int i = 0; i < displaySize; i++)
          displayarray[i] = heart[i];
      } else if (run2 < 30)
      {
        for (int i = 0; i < displaySize; i++)
          displayarray[i] = baum[i];
      } else {
        CalculateTime();
      }
    }
    else {
      CalculateTime();
    }
    if (allOn) {
      ShowAllOn();
    } else {
      ShowTime();
    }

      timeval tv;
      gettimeofday(&tv, nullptr);
      time_t now;
      now = time(nullptr);
      
      // EPOCH+tz+dst
      Serial.print(" gtod:");
      Serial.print((uint32_t)tv.tv_sec);
      Serial.print("/");
      Serial.print((uint32_t)tv.tv_usec);
      Serial.print("us");
      
      // human readable
      Serial.print(" ctime:(UTC+");
      Serial.print((uint32_t)(TZ * 60 + DST_MN));
      Serial.print("mn)");
      Serial.print(ctime(&now));

      // alternative
      Serial.print(hour(now));
      Serial.print(":");
      Serial.print(minute(now));
      Serial.print(":");
      Serial.print(second(now));
      Serial.println();

      if (timeClient.update()){
         Serial.print ( "Adjust local clock" );
         unsigned long epoch = timeClient.getEpochTime();
         // HERE I'M UPDATE LOCAL CLOCK
         setTime(epoch);
         
        // alternative
        Serial.print(hour(now));
        Serial.print(":");
        Serial.print(minute(now));
        Serial.print(":");
        Serial.print(second(now));
        Serial.println();
      }else{
         Serial.print ( "NTP Update not WORK!!" );
      }

      time_t local = CE.toLocal(now);
      Serial.print(hour(local));
      Serial.print(":");
      Serial.print(minute(local));
      Serial.print(":");
      Serial.print(second(local));
      Serial.println();
  }

  delay(10);
  // every 8 hours -> restart
  if (run2 > 2 * 60 * 60 * 8)
  {
      ESP.restart();
  }
}
