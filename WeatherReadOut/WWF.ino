#include <UTFT.h>
#include <UTouch.h>

// Declare which fonts we will be using
extern uint8_t BigFont[]; //16x16
extern uint8_t SevenSegNumFont[]; //32x50
int city, new_city;
int x, y, settings, update_i, temp_i;
String update[24] = {"01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24"};
String temp[3] = {"F","C","K"};

UTFT myGLCD(ITDB50,25,26,27,28);
UTouch myTouch(6,5,4,3,2);

void setup()
{ 
// Setup the LCD
  myGLCD.InitLCD(PORTRAIT);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.clrScr();
  city = 1;
  new_city = 1;
  settings = 0;
  update_i = 0; //index to update array
  temp_i = 0; //index to temp array
}

void loop()
{
  //Clear screen logic
  if(new_city != city) {
    myGLCD.clrScr();
    city = new_city;
  }
  
  myGLCD.setColor(255, 0, 0);
  myGLCD.setFont(BigFont);
  myGLCD.print("Wall Weather Forecaster", CENTER, 0);
  
  //Set the Current Time
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print("10", 150, 80);
  myGLCD.setFont(BigFont);
  myGLCD.print(":", 210, 100);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print("17", 220, 80);
  myGLCD.setFont(BigFont);
  myGLCD.print("PM", 290, 100);
  
  //Print Current City
  if(city == 1) {
    myGLCD.print("Ann Arbor, MI", CENTER, 175);
  }
  else if(city == 2) {
    myGLCD.print("Austin, TX", CENTER, 175);
  }
  else if(city == 3) {
    myGLCD.print("London, England", CENTER, 175);
  }
  else if(city == 4) {
    myGLCD.print("Rio de Janiero, Brazil", CENTER, 175);
  }
  else if(city == 5) {
    myGLCD.print("Los Angeles, CA", CENTER, 175);
  }
  
  //Print Condition
  myGLCD.print("Condition: Snowy as Hell", 0, 240);
  
  //Print High Temperature
  myGLCD.print("High Temp:", 0, 270+30);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print("45", 160, 250+30);
  myGLCD.setFont(BigFont);
  myGLCD.print(temp[temp_i], 220, 250+30);
  
  //Print Low Temperature
  myGLCD.print("Low Temp:", 0, 350+30);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print("28", 160, 330+30);
  myGLCD.setFont(BigFont);
  myGLCD.print(temp[temp_i], 220, 330+30); 
  
  //Print Humidity %
  myGLCD.print("Humidity:", 0, 430+30);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print("56", 160, 410+30);
  myGLCD.setFont(BigFont);
  myGLCD.print("%", 220, 410+30); 
  
  if(settings == 0) {
    //Draw Cities/Settings Boxes
    myGLCD.print("Stored Cities", CENTER, 520+50);
    myGLCD.setFont(SevenSegNumFont);
    if(city == 1) {
      myGLCD.setColor(0, 0, 255); 
      myGLCD.drawRect(51, 550+50, 105, 625+50);
      myGLCD.print("1", 55, 560+50);
      myGLCD.setColor(255, 0, 0); 
      myGLCD.drawRect(132, 550+50, 186, 625+50);
      myGLCD.print("2", 143, 560+50);
      myGLCD.drawRect(213, 550+50, 267, 625+50);
      myGLCD.print("3", 224, 560+50);
      myGLCD.drawRect(294, 550+50, 348, 625+50);
      myGLCD.print("4", 305, 560+50);
      myGLCD.drawRect(375, 550+50, 429, 625+50);
      myGLCD.print("5", 386, 560+50);  
    }
    else if(city == 2) {
      myGLCD.drawRect(51, 550+50, 105, 625+50);
      myGLCD.print("1", 55, 560+50);
      myGLCD.setColor(0, 0, 255); //draw in blue
      myGLCD.drawRect(132, 550+50, 186, 625+50);
      myGLCD.print("2", 143, 560+50);
      myGLCD.setColor(255, 0, 0); //draw in red
      myGLCD.drawRect(213, 550+50, 267, 625+50);
      myGLCD.print("3", 224, 560+50);
      myGLCD.drawRect(294, 550+50, 348, 625+50);
      myGLCD.print("4", 305, 560+50);
      myGLCD.drawRect(375, 550+50, 429, 625+50);
      myGLCD.print("5", 386, 560+50);
      myGLCD.drawRect(375, 550+50, 429, 625+50);   
    }
    else if(city == 3) {
      myGLCD.drawRect(51, 550+50, 105, 625+50);
      myGLCD.print("1", 55, 560+50); 
      myGLCD.drawRect(132, 550+50, 186, 625+50);
      myGLCD.print("2", 143, 560+50);
      myGLCD.setColor(0, 0, 255); //draw in blue
      myGLCD.drawRect(213, 550+50, 267, 625+50);
      myGLCD.print("3", 224, 560+50);
      myGLCD.setColor(255, 0, 0); //draw in red
      myGLCD.drawRect(294, 550+50, 348, 625+50);
      myGLCD.print("4", 305, 560+50);
      myGLCD.drawRect(375, 550+50, 429, 625+50);
      myGLCD.print("5", 386, 560+50);
    }
    else if(city == 4) {
      myGLCD.drawRect(51, 550+50, 105, 625+50);
      myGLCD.print("1", 55, 560+50);
      myGLCD.drawRect(132, 550+50, 186, 625+50);
      myGLCD.print("2", 143, 560+50);
      myGLCD.drawRect(213, 550+50, 267, 625+50);
      myGLCD.print("3", 224, 560+50);
      myGLCD.setColor(0, 0, 255); //draw in blue
      myGLCD.drawRect(294, 550+50, 348, 625+50);
      myGLCD.print("4", 305, 560+50);
      myGLCD.setColor(255, 0, 0); //draw in red
      myGLCD.drawRect(375, 550+50, 429, 625+50);
      myGLCD.print("5", 386, 560+50); 
    }
    else if(city == 5) {
      myGLCD.drawRect(51, 550+50, 105, 625+50);
      myGLCD.print("1", 55, 560+50);
      myGLCD.drawRect(132, 550+50, 186, 625+50);
      myGLCD.print("2", 143, 560+50);
      myGLCD.drawRect(213, 550+50, 267, 625+50);
      myGLCD.print("3", 224, 560+50);
      myGLCD.drawRect(294, 550+50, 348, 625+50);
      myGLCD.print("4", 305, 560+50);
      myGLCD.setColor(0, 0, 255); //draw in blue
      myGLCD.drawRect(375, 550+50, 429, 625+50);
      myGLCD.print("5", 386, 560+50); 
     myGLCD.setColor(255, 0, 0); //draw in red 
    }
    
    myGLCD.setFont(BigFont);
    myGLCD.drawRect(51, 710, 429, 765);
    myGLCD.print("Settings", CENTER, 730);
    
    //TouchScreen Logic
    if(myTouch.dataAvailable()) {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      if(x >= 125 & x <= 200) {
        if(y >= 51 & y <= 105){
          new_city = 1;
        }
        else if(y >= 132 & y <= 186){
          new_city = 2;
        }
        else if(y >= 213 & y <= 267){
          new_city = 3;
        }
        else if(y >= 294 & y <= 348){
          new_city = 4;
        }
        else if(y >= 375 & y <= 429){
          new_city = 5;
        }    
      }
      else if(x >= 35 & x <= 90) {
        if(y >= 51 & y <= 429) {
          settings = 1;  
          myGLCD.clrScr();
        } 
      }
    }
  }
  else if(settings == 1) {
    myGLCD.drawLine(0,515,480,515);
    myGLCD.print("Update Interval:", 0, 555);
    myGLCD.drawRect(265,550,305,580);
    myGLCD.print("<",275,560);
    
    myGLCD.setFont(SevenSegNumFont);
    myGLCD.print(update[update_i], 310, 535);
    
    myGLCD.setFont(BigFont);
    myGLCD.print("hr", 375,555);
    myGLCD.drawRect(410,550,450,580);
    myGLCD.print(">",420,560);
    
    myGLCD.drawRect(80,625,160,675);
    myGLCD.print("F",110,645);
    myGLCD.drawRect(200,625,280,675);
    myGLCD.print("C", 230, 645);
    myGLCD.drawRect(320,625,400,675);
    myGLCD.print("K", 350, 645);
    
    if(myTouch.dataAvailable()) {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      if(x >= 125 & x <= 175) { //temp buttons
        if(y >= 80 & y <= 160){
          temp_i = 0;
        }
        else if(y >= 200 & y <= 280) {
          temp_i = 1; 
        }
        else if(y >= 320 & y <= 400) {
          temp_i = 2; 
        }
      }
      else if(x >= 220 & x <= 250) {
        if(y >= 265 & y <= 305) { //< button pressed
          if(update_i == 0) {
            update_i = 23;
          }
          else {
            update_i = update_i - 1;  
          }
        }
        else if(y >= 410 & y <= 450) { //> button pressed
          if(update_i == 23) {
            update_i = 0;
          }
          else {
            update_i = update_i + 1;
          }  
        }
      }
    }
    //settings = 0;  
  }
}
