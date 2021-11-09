#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "ESP8266httpUpdate.h"
#include "definitions.h"
#ifndef HELPERS_H
#define HELPERS_H


#if defined (__AVR_ATmega328P__)
    char BOARD[]      = {"UNO"};
#elif defined (__AVR_ATmega2560__)
    char BOARD[]      = {"Mega"};
#elif defined (__AVR_ATmega168__)
    char BOARD[]      = {"Mini 168"};
#else
    #warning "device type not defined"
#endif



static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; 
#define LEAP_YEAR(Y) ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )


struct  strDateTime
{
	byte hour;
	byte minute;
	byte second;
	int year;
	byte month;
	byte day;
	byte wday;

} ;



//
// Summertime calculates the daylight saving for a given date.
//
boolean summertime(int year, byte month, byte day, byte hour, byte tzHours)
// input parameters: "normal time" for year, month, day, hour and tzHours (0=UTC, 1=MEZ)
{
 if (month<3 || month>10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
 if (month>3 && month<10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
 if (month==3 && (hour + 24 * day)>=(1 + tzHours + 24*(31 - (5 * year /4 + 4) % 7)) || month==10 && (hour + 24 * day)<(1 + tzHours + 24*(31 - (5 * year /4 + 1) % 7)))
   return true;
 else
   return false;
}

//
// Check the Values is between 0-255
//
boolean checkRange(String Value)
{
	 if (Value.toInt() < 0 || Value.toInt() > 255)
	 {
		 return false;
	 }
	 else
	 {
		 return true;
	 }
}


void WriteStringToEEPROM(int beginaddress, String string)
{
	char  charBuf[string.length()+1];
	string.toCharArray(charBuf, string.length()+1);
	for (int t=  0; t<sizeof(charBuf);t++)
	{
			EEPROM.write(beginaddress + t,charBuf[t]);
	}
}
String  ReadStringFromEEPROM(int beginaddress)
{
	byte counter=0;
	char rChar;
	String retString = "";
	while (1)
	{
		rChar = EEPROM.read(beginaddress + counter);
		if (rChar == 0) break;
		if (counter > 31) break;
		counter++;
		retString.concat(rChar);

	}
	return retString;
}
void EEPROMWritelong(int address, long value)
      {
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }
long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void ConvertUnixTimeStamp( unsigned long TimeStamp, struct strDateTime* DateTime)
{
		uint8_t year;
	uint8_t month, monthLength;
	uint32_t time;
	unsigned long days;
	  time = (uint32_t)TimeStamp;
	  DateTime->second = time % 60;
	  time /= 60; // now it is minutes
	  DateTime->minute = time % 60;
	  time /= 60; // now it is hours
	  DateTime->hour = time % 24;
	  time /= 24; // now it is days
	  DateTime->wday = ((time + 4) % 7) + 1;  // Sunday is day 1 
  
	  year = 0;  
	days = 0;
	while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
		year++;
	}
	DateTime->year = year; // year is offset from 1970 
  
	  days -= LEAP_YEAR(year) ? 366 : 365;
	  time  -= days; // now it is days in this year, starting at 0
  
	  days=0;
	  month=0;
	  monthLength=0;
	  for (month=0; month<12; month++) {
		if (month==1) { // february
		  if (LEAP_YEAR(year)) {
			monthLength=29;
		  } else {
			monthLength=28;
		  }
		} else {
		  monthLength = monthDays[month];
		}
    
		if (time >= monthLength) {
		  time -= monthLength;
		} else {
			break;
		}
	  }
	  DateTime->month = month + 1;  // jan is month 1  
	  DateTime->day = time + 1;     // day of month
	  DateTime->year += 1970;

	 
}


	
String GetMacAddress()
{
	uint8_t mac[6];
    char macStr[18] = {0};
	WiFi.macAddress(mac);
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
    return  String(macStr);
}

String decToHex(byte decValue, byte desiredStringLength) {
  
  String hexString = String(decValue, HEX);
  while (hexString.length() < desiredStringLength) hexString = "0" + hexString;
  
  return hexString;
}

String rgbToString(int r, int g, int b)
{
    return "#" + decToHex(r,2) + decToHex(g,2)+ decToHex( b,2);
}

// convert a single hex digit character to its integer value (from https://code.google.com/p/avr-netino/)
unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

String urldecode(String input) // (based on https://code.google.com/p/avr-netino/)
{
	 char c;
	 String ret = "";
	 
	 for(byte t=0;t<input.length();t++)
	 {
		 c = input[t];
		 if (c == '+') c = ' ';
         if (c == '%') {


         t++;
         c = input[t];
         t++;
         c = (h2int(c) << 4) | h2int(input[t]);
		 }
		
		 ret.concat(c);
	 }
	 return ret;
  
}

void CallUrl(const char * host, String url)
{ 
  WiFiClient client;
  Serial.println(url);
  if (!client.connect(host, 80)) { Serial.println("connection failed"); return; }
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "Connection: close\r\n\r\n");
  
  unsigned long timeout = millis();
  while (client.available() == 0){
    if (millis() - timeout > 5000)
    {
      Serial.println(">>> Client Timeout!");
      client.stop();
      return;
    }
    delay(0); // give the system some time to process other stuff
  }
  while (client.available()) { 
    String line = client.readStringUntil('\r'); 
    Serial.println(line);
  }
  Serial.println("closing connection");
}



int Check(String fwVersionURL)
{
  HTTPClient httpClient;
  httpClient.begin( fwVersionURL );
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


String getMAC()
{
  uint8_t mac[6];
  char result[14];
  snprintf( result, sizeof( result ), "%02x%02x%02x%02x%02x%02x", mac[ 0 ], mac[ 1 ], mac[ 2 ], mac[ 3 ], mac[ 4 ], mac[ 5 ] );
  return String( result );
}

// check for Over The Air Firmware Update
void AutoOta(String localIp, String deviceName, String mac, int fwVersion)
{
  Serial.println("checking ota...");
  String url = "";
  String fwHost = "www.djprime.de";
  url = url + "http://www.djprime.de/IoT/clock.php?key=OtA&s_ip=";url += localIp;
  url += "&devicename=";  url += deviceName;
  url += "&mac="; url+=mac;
  url += "&fwVersion="; url+=fwVersion;
  String fwURL = "/IoT/clock";
  int newVersion = Check(url);
  if( newVersion > FW_VERSION ) {
    Serial.println( "Preparing to update." );
    String fwImageURL = fwURL;
    fwImageURL.concat( newVersion);
    fwImageURL.concat( ".bin" );
    WiFiClient client;
    t_httpUpdate_return ret = ESPhttpUpdate.update( client, fwHost, fwImageURL );
    switch(ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s",  ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;
    }
  }
}
 
#endif
