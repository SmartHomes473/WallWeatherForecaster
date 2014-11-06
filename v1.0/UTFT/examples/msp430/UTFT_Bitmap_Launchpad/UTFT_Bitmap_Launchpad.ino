// UTFT_Bitmap (C)2012 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of the drawBitmap()-function.
//
// This demo was made to work on the 320x240 modules.
// Any other size displays may cause strange behaviour.
//
// This program requires the UTFT library.
//

#include <UTFT.h>

// Declare which fonts we will be using
extern uint8_t SmallFont[];

UTFT myGLCD(S6D1121_8, P1_4, P1_5, P1_6, P1_7);   // Remember to change the model parameter to suit your display module!

extern unsigned short int info[0x400];
extern unsigned short int icon[0x400];
extern unsigned short int tux[0x400];

void setup()
{
  Serial.begin(115200);
  Serial.println("hello world");
  myGLCD.InitLCD();
  Serial.println("initialised lcd");
  myGLCD.setFont(SmallFont);
  Serial.println("set font");
}

void loop()
{
//  myGLCD.fillScr(255, 255, 255);
  myGLCD.fillScr(0, 0, 0);
  Serial.println("filled screen");
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(" *** A 10 by 7 grid of a 32x32 icon *** ", CENTER, 228);
  for (int x=0; x<10; x++)
    for (int y=0; y<7; y++)
      myGLCD.drawBitmap (x*32, y*32, 32, 32, info);
  Serial.println("drew bitmaps");

  delay(5000);
  
  myGLCD.fillScr(255, 255, 255);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("   Two different icons in scale 1 to 4  ", CENTER, 228);
  int x=0;
  for (int s=0; s<4; s++)
  {
    x+=(s*32);
    myGLCD.drawBitmap (x, 0, 32, 32, tux, s+1);
  }
  x=0;
  for (int s=4; s>0; s--)
  {
    myGLCD.drawBitmap (x, 224-(s*32), 32, 32, icon, s);
    x+=(s*32);
  }

  delay(5000);
}

