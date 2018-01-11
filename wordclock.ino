#include "Time.h"
#include "NtpClientLib.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
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

void servedynamic();
void serveupdate();
void clockChange();
void saveSettings();
void servessidlist();

const char* baum  = "     y          g        y g y       ggg      y ggg y     ggggg    y ggggg y   ggggggg  y ggggggg y ggggggggg ";
const char *hiday = "                        w           w ww        w           w           w           w                         "; 
const char *heart = "  rrr rrr   rrrrrrrrr rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr rrrrrrrrr   rrrrrrr     rrrrr       rrr         r     ";
const char* words = "ESKISTMFUNFZEHNZWANZIGAPDJVIERTELVORAQHINACHHALBYELFUNFEINSWAXZWEIDREIAPNVIERSECHSGLACHTSIEBENZWOLFZEHNEUNTUHR";
//                   01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
//                   00000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990000000000
const byte word_es[2] = {0,1};
const byte word_ist[2] = {3,5};
const byte word_uhr[2] = {107,109};
const byte word_m_fuenf[2] = {7,10};
const byte word_m_zehn[2] = {11,14};
const byte word_m_viertel[2] = {26,32};
const byte word_m_zwanzig[2] = {15,21};
const byte word_m_vor[2] = {33,35};
const byte word_m_nach[2] = {40,43};
const byte word_m_halb[2] = {44,47};

const byte word_h_ein[2] = {55,57};
const byte word_h_eins[2] = {55,58};
const byte word_h_zwei[2] = {62,65};
const byte word_h_drei[2] = {66,69};
const byte word_h_vier[2] = {73,76};
const byte word_h_fuenf[2] = {51,54};
const byte word_h_sechs[2] = {77,81};
const byte word_h_sieben[2] = {88,93};
const byte word_h_acht[2] = {84,87};
const byte word_h_neun[2] = {102,105};
const byte word_h_zehn[2] = {99,102};
const byte word_h_elf[2] = {49,51};
const byte word_h_zwoelf[2] = {94,98};

//const byte* word_hours[12] = {word_h_zwoelf, word_h_eins, word_h_zwei, word_h_drei, word_h_vier, word_h_fuenf, word_h_sechs, word_h_sieben, word_h_acht, word_h_neun, word_h_zehn, word_h_elf};
const byte word_hours[12][2] = {{94,98},{55,58},{62,65},{66,69},{73,76},{51,54},{77,81},{88,93},{84,87},{102,105},{99,102},{49,51}};


char displayarray[111];
void initDisplay()
{
  for (int i=0;i<110;i++)
  {
    displayarray[i]=32;
  }
  displayarray[110]=0;
}

void updateDisplay(byte from, byte to)
{
  for (int i=from; i<=to;i++)
  {
    displayarray[i]=words[i];
  }
}

#define StripPin D5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(110, StripPin, NEO_GRB + NEO_KHZ800);

int ledPin = D0; // GPIO16

//WiFiServer server(80); //Initialize the server on Port 80
ESP8266WebServer server(80);

void setDefaultConfig()
{
    // DEFAULT CONFIG
    config.ssid = "clock";
    config.password = "clock";
    config.dhcp = true;
    config.ap = true;
    config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
    config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
    config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
    config.ntpServerName = "0.de.pool.ntp.org";
    config.Update_Time_Via_NTP_Every =  0;
    config.timezone = +1;
    config.daylight = true;
    config.DeviceName = "clock";  
    config.Color_R = 255;
    config.Color_G = 255;
    config.Color_B = 255;
    config.Hue = 255;
    config.Dynamic = 128;
}

/*
 * small helper function to test the complete strip
 */
int run = 0;
void Lauflicht()
{
    for (int i = 0; i < strip.numPixels(); i++) 
    {
      uint32_t co = 0;
      if (run % strip.numPixels() == i)
      {
        co = strip.Color((255-run % 255)/2,(255- run % 255)/2,(run % 255)/2);
      }
      if ((run+1) % strip.numPixels() == i || (run-1) % strip.numPixels() == i)
      {
        co = strip.Color((255-run % 255)/8,(255- run % 255)/8,(run % 255)/8);
      }                 
      strip.setPixelColor(i, co);
    }
    strip.show();  
}

void initWifi()
{
  WiFi.hostname("clock");
  wifi_station_set_hostname("clock");

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


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //WiFi.start();

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (counter>10)
    {
      config.ap=true;
      config.ssid="clock";
      config.password="clock";
      initWifi();
      break;
    }
    Lauflicht();
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
}

#define webblocksize 3000
void sendfile(const prog_char str[])
{
  String result = "";
  if(!str) return;
  char c;
  int pos = 0;
  if (server.hasArg("pos"))
  {
    str+=server.arg("pos").toInt();    
  }
  while((c = pgm_read_byte(str)) && pos<webblocksize)
  {
    result+=c;
    str++;
    pos++;
  }
  server.send ( 200, "text/plain", result);   
  /*
  char *buffer = new char[1000]();
  strncpy((char*)was, buffer, 1000);
  //strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i])));
  server.send(200,"text/html", buffer);
  */
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

  if (name=="jscolor.js") {
    serve = (char*)jscolor_js;    
  } else if ( name== "body.js" ) {
    serve = (char*)body_js;
  } else if ( name== "body.html" ) {
    serve = (char*)body_html;
  } else if ( name== "microajax.js" ) {
    serve = (char*)microajax_js;
  } else if ( name== "milligram.min.css" ) {
    serve = (char*) milligram_min_css;
  } else {
    Serial.print("name not found - ");
  }
  
  if (serve == 0)
  {
    server.send(404, "text/plain", "file not found");
    return;
  }

  int len=0;
  len = strlen_P(serve);

  String result = "";
  result.reserve(4001);
  char myChar;
  int pos = start;
  
  for (int i=0;i<4000 && pos<len; i++)
  {    
    myChar = pgm_read_byte (serve+pos);      
    result += myChar;
    pos++;
  }
  server.send(200, "text/plain", result);

  Serial.print("load ");
  Serial.print(name);
  Serial.print(" from ");
  Serial.print(start);
  Serial.print(" length ");
  Serial.println(result.length());
  //Serial.println(result);
}

/*
 * return the current clock display for website display
 */
void serveclock()
{ 
    String result = "";    
    for (int i = 0; i < strip.numPixels(); i++) 
    {
        int di=i;
        if ((i/11) % 2 == 1)
        {
          //reverse position
          di = (i / 11)*11 + 10-i % 11;
        }
        
       result+=words[i];
       uint32_t color = strip.getPixelColor(di);
       int b = color & 0xFF;
       int g = (color >> 8) & 0xFF;
       int r = (color >> 16) & 0xFF;
       result += rgbToString(r,g,b);
    }
    
    //CalculateTime();
    server.send ( 200, "text/plain", result);   
}


void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  delay(10);
	Serial.println("Starting ES8266");

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

  initWifi();
   
  // Start the server
  server.on ( "/", []() { server.send ( 200, "text/html", index_html );  } );

  server.on ( "/index.html", []() { server.send ( 200, "text/html", index_html );  } );
  server.on ( "/microajax.js", []() { server.send ( 200, "text/html", microajax_js );  } );
  server.on ( "/milligram.min.css", []() { server.send ( 200, "text/html", milligram_min_css );  } );
  server.on ( "/jscolor.js", []() { server.send ( 200, "text/html", jscolor_js );  } );
  server.on ( "/ajax.js", []() { server.send ( 200, "text/html", ajax_js );  } );
  server.on ( "/body.js", []() { server.send ( 200, "text/html", body_js );  } );
  server.on ( "/body.html", []() { server.send ( 200, "text/html", body_html );  } );

  server.on ( "/load", serveParts );

  server.on ( "/serve/clock", serveclock );
  server.on ( "/serve/dynamic", servedynamic );
  server.on ( "/serve/update", serveupdate );
  server.on ( "/serve/changeTime", clockChange );
  server.on ( "/serve/SaveSettings", saveSettings);
  server.on ( "/serve/ssidlist", servessidlist);
  
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

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
    NTP.begin("pool.ntp.org", 1, true);
    NTP.setInterval(1800);

    strip.begin();
    strip.show();   
    digitalWrite(ledPin, HIGH);
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
 * calculate the correct time
 */
void CalculateTime()
{
  initDisplay();
  int h = hour();
  if (h>11) h -=12;
  int m = minute();

  updateDisplay(word_es[0], word_es[1]);
  updateDisplay(word_ist[0], word_ist[1]);

  int hadd = h+1;
  if (hadd==12)
    hadd=0;

  if (m<3 || m>=58)
  {
    if (h==1)
    {
      updateDisplay(word_h_ein[0], word_h_ein[1]);      
    }
    else
    {
      updateDisplay(word_hours[h][0], word_hours[h][1]);      
    }
    updateDisplay(word_uhr[0], word_uhr[1]);      
  } else if (m<8) {
    updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_hours[h][0], word_hours[h][1]);
  } else if (m<13) {    
    updateDisplay(word_m_zehn[0], word_m_zehn[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_hours[h][0], word_hours[h][1]);
  } else if (m<18) {    
    updateDisplay(word_m_viertel[0], word_m_viertel[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_hours[h][0], word_hours[h][1]);
  } else if (m<23) {    
    updateDisplay(word_m_zwanzig[0], word_m_zwanzig[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_hours[h][0], word_hours[h][1]);
  } else if (m<28) {    
    updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
    updateDisplay(word_m_vor[0], word_m_vor[1]);
    updateDisplay(word_m_halb[0], word_m_halb[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else if (m<33) {    
    updateDisplay(word_m_halb[0], word_m_halb[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else if (m<38) {    
    updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_m_halb[0], word_m_halb[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else if (m<43) {    
    updateDisplay(word_m_zehn[0], word_m_zehn[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_m_halb[0], word_m_halb[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else if (m<48) {    
    updateDisplay(word_m_viertel[0], word_m_viertel[1]);
    updateDisplay(word_m_vor[0], word_m_vor[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else if (m<53) {    
    updateDisplay(word_m_zehn[0], word_m_zehn[1]);
    updateDisplay(word_m_vor[0], word_m_vor[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  } else if (m<58) {    
    updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
    updateDisplay(word_m_vor[0], word_m_vor[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  }

  //updateDisplay(word_uhr[0], word_uhr[1]);

  Serial.println(displayarray);
}



long envhue;
/*
 * return the dynamic values for the website
 */
void servedynamic()
{
    String values ="";
    values += "ssid|"+config.ssid+"\n";
    values += "password|"+config.password+"\n";
    values += "color|"+rgbToString(config.Color_R, config.Color_G, config.Color_B)+"\n";
    values += "_hour|" + String(hour())+"|text\n";
    values += "_minute|" + String(minute()) + "|text\n";
    values += "hue|"+String(config.Hue)+"\n";
    values += "dynamic|"+String(config.Dynamic)+"\n";
    values += "envhue|"+String((envhue*100) >> 10 )+"%|text\n";
    server.send ( 200, "text/plain", values);   
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

void servessidlist()
{
  String result = "";
  byte numSsid = WiFi.scanNetworks();
  for (int i=0;i<numSsid;i++){
    result+=WiFi.SSID(i)+"|"+WiFi.SSID(i)+" ("+WiFi.RSSI(i)+" dBm - "+MapEncryptionType(WiFi.encryptionType(i))+")\n";
  }
  server.send(200, "text/plain", result);
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
            int v = 1;
            if (server.arg(1)=="-1")
            {
              v=-1;          
            }
            if (server.arg(0)=="hour")
            {
                adjustTime(v*60*60);
            } else if (server.arg(0)=="minute")
            {
                adjustTime(v*60);            
            }
         }        
      }
   }
   server.send( 200, "text/plain", "ok");
}

void saveSettings()
{
  WriteConfig();
  server.send( 200, "text/plain", "ok");
  ESP.restart();
}
/*
 * 
 */
void serveupdate()
{
    Serial.println("update!");
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
        if (server.hasArg("hue")) config.Hue = server.arg("hue").toInt();    
        if (server.hasArg("dynamic")) config.Dynamic = server.arg("dynamic").toInt();          
        if (server.hasArg("apmode")) config.ap = (server.arg("apmode")=="true");
        if (server.hasArg("ssid") && config.ap) config.ssid = server.arg("ssid");
        if (server.hasArg("ssidlist") && !config.ap) config.ssid=server.arg("ssidlist");
        if (server.hasArg("password")) config.password=server.arg("password");
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
        Serial.print("dynamic:"); Serial.println(config.Dynamic);
        Serial.print("apMode:"); Serial.println(config.ap);
        Serial.print("ssid:"); Serial.println(config.ssid);
        Serial.print("password:"); Serial.println(config.password);
    }
    server.send( 200, "text/plain", "ok");
}

/*
 * helper function to update the time
 */
int _timeUpdateCounter = 0;
void TimeUpdate()
{
  _timeUpdateCounter++;
  if (_timeUpdateCounter & 127 != 1)
  {
    return;
  }

  Serial.println("loading new time");
  time_t t = NTP.getTime();
  setTime(t);
}



/*
 * Display the DisplayArray to the Matrix LED-Display
 */
void ShowTime()
{
  // calculate dynamic lightning part regarding environmental hue and config settings
  long dyn = config.Dynamic;  
  dyn = 255-dyn; //((long) dyn) * ((long)envhue) >> 10 + 255-dyn;
  long etmp = envhue;
  etmp = (etmp * ((long)config.Dynamic)) >> 10;
  dyn += etmp;
  // calculate base hue regarding config and dynamic lightning
  long h = (((long) config.Hue)*dyn) >> 8;
  
    Serial.print("base hue: ");
    Serial.println(h);
    Serial.println(dyn);
    Serial.println(config.Hue);
    Serial.println(config.Dynamic);
  
  
  int cr = ((int) config.Color_R * (int) h) >> 8;
  int cg = ((int) config.Color_G * (int) h) >> 8;
  int cb = ((int) config.Color_B * (int) h) >> 8;
  uint32_t coHigh = strip.Color(cr,cg,cb);

  uint32_t coRed = strip.Color(h,0,0);
  uint32_t coGreen = strip.Color(0,h,0);
  uint32_t coYellow = strip.Color(h, h, 0);

  char redChar  = 'r';
  char greenChar = 'g';
  char yellowChar = 'y';
     
  for (int i=0;i<110;i++)
  {
      int di=i;
      if ((i/11) % 2 == 1)
      {
        //reverse position
        di = (i / 11)*11 + 10-i % 11;
      }
      uint32_t co = 0;
      if (displayarray[di]!=32){
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
      strip.setPixelColor(i, co);
  }
  displayarray[110]=0;
  strip.show();  
}


int run2 = 0;

void loop() {
  run++;   
  server.handleClient();  
  //Lauflicht();
  envhue = analogRead(A0);
  if (run % 5 == 1)
  {
    run2++;
    /*
    Serial.print("environment lightning: ");
    Serial.println(envhue);
    */

    if (run2 % 5 == 1) TimetoConsole();
    
    if (run2<10)
    {
      for (int i=0;i<110;i++)
        displayarray[i] = hiday[i];
    } else if (run2<20)
    {
      for (int i=0;i<110;i++)
        displayarray[i] = heart[i];
    } else if (run2<30)
    {
      for (int i=0;i<110;i++)
        displayarray[i] = baum[i];
    } else {  
      CalculateTime();  
    }
    ShowTime();    
  }  
  delay(100); 
}

