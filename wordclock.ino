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

const char* baum = "     G          g        G g G       ggg      G ggg G     ggggg    G ggggg G   ggggggg  G ggggggg G ggggggggg ";

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
const byte word_hours[12][2] = {{94,98},{55,57},{62,65},{66,69},{73,76},{51,54},{77,81},{88,93},{84,87},{102,105},{99,102},{49,51}};


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
  /*
  Serial.print("word ");
  Serial.print(from);
  Serial.print(" to ");
  Serial.println(to);
  */
}

/*
void initClock()
{
  words[0,0] = word_m_fuenf; words[0,1] = word_m_nach;
  words[1,0] = word_m_zehn; words[1,1] = word_m_nach;
  words[2,0] = word_m_viertel; words[2,1] = word_m_nach;
  words[3,0] = word_m_zwanzig; words[3,1] = word_m_nach;
  words[4,0] = word_m_fuenf; words[4,1] = word_m_vor; words[5,1] = word_m_halb;
}

*/




#define StripPin D6
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
}

void initWifi()
{
  const char*ssid = config.ssid.c_str();
  const char*password = config.password.c_str();

  if (config.ap)
  {
    WiFi.mode(WIFI_AP);    
    WiFi.softAP(ssid, password);
    return;
  }
  else
  {
    WiFi.mode(WIFI_STA);
  }
  
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
 
  wifi_station_set_hostname("clock");

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

  if (m<3)
  {
    updateDisplay(word_hours[h][0], word_hours[h][1]);
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
  } else {
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
    updateDisplay(word_uhr[0], word_uhr[1]);
  }

  //updateDisplay(word_uhr[0], word_uhr[1]);

  Serial.println(displayarray);
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

  if (name=="jscolor.js")
  {
    serve = (char*)jscolor_js;    
  } else if (name=="body.js")
  {
    serve = (char*)body_js;
  } else if (name=="body.html")
  {
    serve = (char*)body_html;
  } else if (name="microajax.js")
  {
    serve = (char*)microajax_js;
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
  Serial.println(result);
}

/*
 * return the current clock display for website display
 */
void serveclock()
{ 
    String result = "";    
    for (int i = 0; i < strip.numPixels(); i++) 
    {
       result+=words[i];
       uint32_t color = strip.getPixelColor(i);
       int r = color & 0xFF;
       int g = (color > 8) & 0xFF;
       int b = (color > 16) & 0xFF;
       result += rgbToString(r,g,b);
    }
    
    //CalculateTime();
    server.send ( 200, "text/plain", result);   
}

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
    server.send ( 200, "text/plain", values);   
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
}
/*
 * 
 */
void serveupdate()
{
    Serial.println("update!");
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
          if (server.argName(i) == "color")
          {
              String co = urldecode(server.arg(i));
              if (co.length()==7 && co[0]=='#')
              {
                 config.Color_R = h2int(co[1]) *16 +h2int(co[2]);
                 config.Color_G = h2int(co[3]) *16 +h2int(co[4]);                 
                 config.Color_B = h2int(co[5]) *16 +h2int(co[6]);                 
              }
          } else if (server.argName(i) == "hue") {
             config.Hue = server.arg(i).toInt();
          } else if (server.argName(i) == "dynamic") {
             config.Dynamic = server.arg(i).toInt();
          } else if (server.argName(i) == "ssid") {
             config.ssid = server.arg(i);
          } else if (server.argName(i) == "password") {
             config.password = server.arg(i);
          }
          Serial.println(server.argName(i));
          Serial.println(server.arg(i));
        }
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

/*
 * Display the DisplayArray to the Matrix LED-Display
 */
void ShowTime()
{
  uint32_t coHigh = strip.Color(4,4,4);
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
        co = coHigh;
      }
      strip.setPixelColor(i, co);
  }
  displayarray[110]=0;
  strip.show();  
}



void loop() {
  run++;   
  server.handleClient();  
  //Lauflicht();
  if (run % 50 == 1)
  {
    TimetoConsole();
    CalculateTime();  
    ShowTime();
  }  
  delay(20); 
}

