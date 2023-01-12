/*
 */

#ifndef WATCH_H
#define WATCH_H

#include <Adafruit_NeoPixel.h>
#include "config.h"


#define displaySize 114

bool WatchAllOn = false;

int LoopCounter = 0;
int LoopCounter2 = 0;

/*
all watch definitions
 */

char *letters = "abcdefghijklmnopqrstuvwxyz0123456789";
const char* baum  = "     y          g        y g y       ggg      y ggg y     ggggg    y ggggg y   ggggggg  y ggggggg y ggggggggg ....";
const char* hiday = "                        W           W WW        W           W           W           W                             ";
const char* heart = "  rrr rrr   rrrrrrrrr rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr rrrrrrrrr   rrrrrrr     rrrrr       rrr         r     rrrr";
const char* words = "ESKISTMFUNFZEHNZWANZIGAPDJVIERTELVORAQHINACHHALBYELFUNFEINSWAXZWEIDREIAPNVIERSECHSGLACHTSIEBENZWOLFZEHNEUNTUHR    ";
const char* helper= "ESKISTMFUNFZEHNZWANZIGAPDJVIERTELVORAQHINACHHALBYELFUNFEINSWAXZWEIDREIAPNVIERSECHSGLACHTSIEBENZWOLFZEHNEUNTUHR    ";      
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

const byte word_hours[12][2] = {{94, 98}, {55, 58}, {62, 65}, {66, 69}, {73, 76}, {51, 54}, {77, 81}, {88, 93}, {84, 87}, {102, 105}, {99, 102}, {49, 51}};


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
    if (i < 0 || i > 114)
    {
      Serial.print("trying to write to display on ");
      Serial.print(i);
      Serial.println();
      return;
    }
    displayarray[i] = words[i];
  }
}
#define StripPin D5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(displaySize * 2, StripPin, NEO_GRB + NEO_KHZ800);

#define ledPin D4



void Lauflicht()
{
  LoopCounter++;
  int lc2 = LoopCounter / 10;
  for (int i = 0; i < strip.numPixels(); i++)
  {
    uint32_t co = 0;
    if (lc2 % strip.numPixels() >= i - 2 && lc2 % strip.numPixels() <= i + 2)
    {
      co = strip.Color(64, 64, 64);
    }
    strip.setPixelColor(i, co);
  }
  strip.show();
}

void Lauflicht2()
{
  LoopCounter++;
  for (int i = 0; i < strip.numPixels(); i++)
  {
    uint32_t co = 0;
    if (LoopCounter % strip.numPixels() == i)
    {
      co = strip.Color((255 - LoopCounter % 255) / 2, (255 - LoopCounter % 255) / 2, (LoopCounter % 255) / 2);
    }
    if ((LoopCounter + 1) % strip.numPixels() == i || (LoopCounter - 1) % strip.numPixels() == i)
    {
      co = strip.Color((255 - LoopCounter % 255) / 8, (255 - LoopCounter % 255) / 8, (LoopCounter % 255) / 8);
    }
    strip.setPixelColor(i, co);
  }
  strip.show();
}

void ShowAllOn()
{
  long h = ((long) config.Hue);
  
  int cr = h;
  int cg = h;
  int cb = h;

   // do a color change to ensure data transfer is working and also all LEDs are working correct
  int com = LoopCounter2 / 2;

  if ((com & 1) > 0)
  {
    cr = 0;
  }
  if ((com & 2) > 0)
  {
    cg = 0;
  }
  if ((com & 4) > 0)
  {
    cb = 0;
  }
  
  uint32_t coHigh = strip.Color(cr, cg, cb);
  Serial.print("all lights mode - loop ");
  Serial.print(com);
  Serial.print(" - color ");
  Serial.println(coHigh);

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
  long h = ((long) config.Hue);
  long h2 = h >> 1;

  Serial.print("base hue: ");
  Serial.println(h);
  Serial.println(config.Hue);

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

void TimetoConsole()
{
  /*
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());  
  */

  Serial.print(tm.tm_hour);
  Serial.print(":");
  Serial.print(tm.tm_min);
  Serial.print(":");
  Serial.println(tm.tm_sec);  

}

/*
   calculate the correct time
*/

void SetTimeToDisplay(int h, int m)
{
  int hadd = h + 1;
  if (hadd == 12)
    hadd = 0;

  if (config.itison==true)
  {
    updateDisplay(word_es[0], word_es[1]);
    updateDisplay(word_ist[0], word_ist[1]);    
  } 
  if (m>=0 && m<5)
  {
    updateDisplay(word_es[0], word_es[1]);
    updateDisplay(word_ist[0], word_ist[1]);        
  }  
  if (m>=30 && m<35)
  {
    updateDisplay(word_es[0], word_es[1]);
    updateDisplay(word_ist[0], word_ist[1]);            
  }

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
    return;
  }
  
  if (m < 10) {
    updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_hours[h][0], word_hours[h][1]);
    return;
  }

  if (m < 15) {
    updateDisplay(word_m_zehn[0], word_m_zehn[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_hours[h][0], word_hours[h][1]);     
    return;
  }

  if (m < 20) {
    if (config.speechmode){      
      updateDisplay(word_m_viertel[0], word_m_viertel[1]);      
      updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
    } else {
      updateDisplay(word_m_viertel[0], word_m_viertel[1]);
      updateDisplay(word_m_nach[0], word_m_nach[1]);
      updateDisplay(word_hours[h][0], word_hours[h][1]);      
    }    
    return;
  }  

  if (m < 25) {
    if (config.speechmode){      
      updateDisplay(word_m_zehn[0], word_m_zehn[1]);  
      updateDisplay(word_m_vor[0], word_m_vor[1]);
      updateDisplay(word_m_halb[0], word_m_halb[1]);
      updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);     
    } else {
      updateDisplay(word_m_zwanzig[0], word_m_zwanzig[1]);
      updateDisplay(word_m_nach[0], word_m_nach[1]);
      updateDisplay(word_hours[h][0], word_hours[h][1]);
    }
    return;
  }  
  
  if (m < 30) {
    updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
    updateDisplay(word_m_vor[0], word_m_vor[1]);
    updateDisplay(word_m_halb[0], word_m_halb[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
    return;
  }

 if (m < 35) {
    updateDisplay(word_m_halb[0], word_m_halb[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
    return;
  } 
  
  if (m < 40) {
    updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
    updateDisplay(word_m_nach[0], word_m_nach[1]);
    updateDisplay(word_m_halb[0], word_m_halb[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
    return;
  } 
  
  if (m < 45) {
    if (config.speechmode){      
      updateDisplay(word_m_zwanzig[0], word_m_zwanzig[1]);
      updateDisplay(word_m_vor[0], word_m_vor[1]);
      updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);     
    } else {
      updateDisplay(word_m_zehn[0], word_m_zehn[1]);
      updateDisplay(word_m_nach[0], word_m_nach[1]);
      updateDisplay(word_m_halb[0], word_m_halb[1]);
      updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
    }
    return;
  } 
  
  if (m < 50) {
    updateDisplay(word_m_viertel[0], word_m_viertel[1]);
    updateDisplay(word_m_vor[0], word_m_vor[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
    return;
  } 
  
  if (m < 55) {
    updateDisplay(word_m_zehn[0], word_m_zehn[1]);
    updateDisplay(word_m_vor[0], word_m_vor[1]);
    updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
    return;
  } 
  
  updateDisplay(word_m_fuenf[0], word_m_fuenf[1]);
  updateDisplay(word_m_vor[0], word_m_vor[1]);
  updateDisplay(word_hours[hadd][0], word_hours[hadd][1]);
  
  
}

void CalculateTime()
{
  initDisplay();
  int h = tm.tm_hour; //timeClient.getHours();
  if (h > 11) h -= 12;
  int m = tm.tm_min; //timeClient.getMinutes();

  SetTimeToDisplay(h, m);
  
  int l;
  for (l = 0; l < m % 5; l ++)
  {
    displayarray[l + 110] = '.';
  }
}



void WatchLoop()
{
  LoopCounter++;

  if (LoopCounter < 200)
  {
    Lauflicht();
    return;
  }

  if (config.DeviceName == "clock_dayan")
  {
    if (LoopCounter < 400)
    {
      for (int i=0;i<displaySize ;i++)
      {
        displayarray[i] = hiday[i];
      }
      ShowTime();     
      return;
    } else if (LoopCounter < 600)
    {
      for (int i=0;i<displaySize ;i++)
      {
        displayarray[i] = heart[i];
      }
      ShowTime();     
      return;      
    }
  }

  if (LoopCounter % 40 == 0)
  {
    LoopCounter2++;
    if (LoopCounter2 % 64 == 1) TimetoConsole();

    CalculateTime(); 
    if (WatchAllOn) {
       ShowAllOn();
    } else {
       ShowTime();
    }   
    return;
  }
}

#endif
