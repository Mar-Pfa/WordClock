#include "helpers.h"

struct strConfig {
	  String ssid;      // ssid name of then network
	  String password;  // password for connection
	  byte  IP[4];      // ip adress
	  byte  Netmask[4]; // network mask
	  byte  Gateway[4]; // default gateway adress
	  boolean dhcp;
    boolean ap; // act as access point
	  String ntpServerName;
	  long Update_Time_Via_NTP_Every;
	  long timezone; 
	  String DeviceName; // devicename for the clock in the network
	  byte Color_R; // red color component for the clock
	  byte Color_G; // green color component for the clock
	  byte Color_B; // blue color component for the clock
    byte Dynamic; // dynamic rate from 0 to 255
    byte Hue;     // hue
    byte daylight; // daylight saving
}   config;


void ConfigureWifi()
{
	Serial.println("Configuring Wifi");
	WiFi.begin (config.ssid.c_str(), config.password.c_str());
	if (!config.dhcp)
	{
		WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ));
	}
}

void WriteConfig()
{

	Serial.println("Writing Config");
	EEPROM.write(0,'C');
	EEPROM.write(1,'L');
	EEPROM.write(2,'O');
	EEPROM.write(3,'C');
	EEPROM.write(4,'K');

	EEPROM.write(16,config.dhcp);
	EEPROM.write(17,config.daylight);
	
	EEPROMWritelong(18,config.Update_Time_Via_NTP_Every); // 4 Byte

	EEPROMWritelong(22,config.timezone);  // 4 Byte


	EEPROM.write(26,config.Color_R);
	EEPROM.write(27,config.Color_G);
	EEPROM.write(28,config.Color_B);
  EEPROM.write(29,config.Dynamic);
  EEPROM.write(30,config.Hue);
  EEPROM.write(31,config.ap);
	EEPROM.write(32,config.IP[0]);
	EEPROM.write(33,config.IP[1]);
	EEPROM.write(34,config.IP[2]);
	EEPROM.write(35,config.IP[3]);

	EEPROM.write(36,config.Netmask[0]);
	EEPROM.write(37,config.Netmask[1]);
	EEPROM.write(38,config.Netmask[2]);
	EEPROM.write(39,config.Netmask[3]);

	EEPROM.write(40,config.Gateway[0]);
	EEPROM.write(41,config.Gateway[1]);
	EEPROM.write(42,config.Gateway[2]);
	EEPROM.write(43,config.Gateway[3]);


	WriteStringToEEPROM(64,config.ssid);
	WriteStringToEEPROM(96,config.password);
	WriteStringToEEPROM(128,config.ntpServerName);

	WriteStringToEEPROM(306,config.DeviceName);
	


	EEPROM.commit();
}

boolean ReadConfig()
{
	Serial.println("Reading Configuration");
	if (EEPROM.read(0) != 'C' || EEPROM.read(1) != 'L'  || EEPROM.read(2) != 'O' || EEPROM.read(3) != 'C' || EEPROM.read(4) != 'K' )
	{
		Serial.println("Configuration NOT FOUND!!!!");
		return false;
	}
    Serial.println("Configurarion Found!");
    config.dhcp = 	EEPROM.read(16);

    config.daylight = EEPROM.read(17);

    config.Update_Time_Via_NTP_Every = EEPROMReadlong(18); // 4 Byte

    config.timezone = EEPROMReadlong(22); // 4 Byte

    config.Color_R = EEPROM.read(26);
    config.Color_G = EEPROM.read(27);
    config.Color_B = EEPROM.read(28);
    config.Dynamic = EEPROM.read(29);
    config.Hue = EEPROM.read(30);

    config.ap = EEPROM.read(31);
    config.IP[0] = EEPROM.read(32);
    config.IP[1] = EEPROM.read(33);
    config.IP[2] = EEPROM.read(34);
    config.IP[3] = EEPROM.read(35);
    config.Netmask[0] = EEPROM.read(36);
    config.Netmask[1] = EEPROM.read(37);
    config.Netmask[2] = EEPROM.read(38);
    config.Netmask[3] = EEPROM.read(39);
    config.Gateway[0] = EEPROM.read(40);
    config.Gateway[1] = EEPROM.read(41);
    config.Gateway[2] = EEPROM.read(42);
    config.Gateway[3] = EEPROM.read(43);
    config.ssid = ReadStringFromEEPROM(64);
    config.password = ReadStringFromEEPROM(96);
    config.ntpServerName = ReadStringFromEEPROM(128);
            
    config.DeviceName= ReadStringFromEEPROM(306);
    return true;		
}
