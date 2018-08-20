//**************************************************************************************************
//                                  3-line and 4-line LCD NUMBERS 
//                                   Adrian Jones, March 2015
//
//**************************************************************************************************


#include <Wire.h> 
#include <TinyGPS++.h>
#include <LiquidCrystal_I2C.h>
#include <SFE_BMP180.h>
LiquidCrystal_I2C lcd(0x27,20,4);
TinyGPSPlus gps;
SFE_BMP180 pressure;

const char custom[][8] PROGMEM = {
     {0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F},      // char 1: bottom right triangle
     {0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F},      // char 2: bottom block
     {0x10, 0x1C, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F},      // char 3: bottom left triangle
     {0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00},      // char 4: top right triangle
     {0x1F, 0x1E, 0x1C, 0x10, 0x00, 0x00, 0x00, 0x00},      // char 5: top left triangle
     {0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00},      // char 6: upper block
     {0x1F, 0x1F, 0x1E, 0x1C, 0x18, 0x18, 0x10, 0x10},      // char 7: full top right triangle
     {0x01, 0x07, 0x0F, 0x1F, 0x00, 0x00, 0x00, 0x00}       // char 8: top right triangle
};
 
const char bn3[][30] PROGMEM = {                            // 3-line numbers
//         0               1               2               3               4              5               6                7               8               9
    {0x01,0x06,0x03, 0x08,0xFF,0xFE, 0x08,0x06,0x03, 0x08,0x06,0x03, 0xFF,0xFE,0xFF, 0xFF,0x06,0x06, 0x01,0x06,0xFE, 0x06,0x06,0xFF, 0x01,0x06,0x03, 0x01,0x06,0x03},
    {0xFF,0xFE,0xFF, 0xFE,0xFF,0xFE, 0x01,0x06,0x06, 0xFE,0x06,0xFF, 0x06,0x06,0xFF, 0x06,0x06,0xFF, 0xFF,0x06,0x03, 0xFE,0x01,0x07, 0xFF,0x06,0xFF, 0x04,0x06,0xFF},
    {0x04,0x06,0x05, 0xFE,0x06,0xFE, 0x06,0x06,0x06, 0x04,0x06,0x05, 0xFE,0xFE,0x06, 0x04,0x06,0x05, 0x04,0x06,0x05, 0xFE,0x06,0xFE, 0x04,0x06,0x05, 0xFE,0xFE,0x06}
};

byte col,row,nb=0,bc=0;                                   // general0xFE,0x01,0x07
byte bb[8];                                               // byte buffer for reading from PROGMEM

int hour, minute, second, hour2, day, oday, omonth, oyear;
boolean Date = false;
byte hr, mn, se, ohr, omn, osec;
char status;
double T,P,p0,a,F;
unsigned long currentMillis, tempMillis;
const int DSTPin = 2;     // the number of the pushbutton pin
int buttonState = 0;

//*****************************************************************************************//
//                                      Initial Setup
//*****************************************************************************************//
void setup() {
  tempMillis = millis();
  Serial3.begin(115200);
  pressure.begin();
  randomSeed(analogRead(0));
  lcd.init();
  lcd.backlight();
  for (nb=0; nb<8; nb++ ) {                     // create 8 custom characters
    for (bc=0; bc<8; bc++) bb[bc]= pgm_read_byte( &custom[nb][bc] );
    lcd.createChar ( nb+1, bb );
  }
  lcd.clear();
  lcd.setCursor(4, 0); 
  lcd.print(F("TylerBennett")); 
  lcd.setCursor(5, 1); 
  lcd.print(F("GPS & Temp")); 
  lcd.setCursor(5, 2); 
  lcd.print(F("Desk Clock"));
  lcd.setCursor(5, 3); 
  lcd.print(F("08/11/2018"));
  printNum3(random(0,10), 0, 0);
  printNum3(random(0,10),17, 0);
  delay(2500);
  lcd.clear();
  //printColon(6, 0);
  //printColon(13, 0); 
  pinMode(DSTPin, INPUT_PULLUP); 


  
 }
 
//*****************************************************************************************//
//                                      MAIN LOOP
//*****************************************************************************************//
void loop() {
  currentMillis = millis();
  
  while (Serial3.available() > 0){
    if (gps.encode(Serial3.read())){
      if (gps.time.isValid()){
        buttonState = digitalRead(DSTPin);
        hour = gps.time.hour();
        mn = gps.time.minute();
        se = gps.time.second();
        
        if (buttonState == HIGH){
          hour2 = hour-4;
        }
        else {
          hour2 = hour-5;
        }
        
        if (hour2 < 0){
          hour2 = hour2+24;
          Date = true;
        }
        
        if (hour2 >= 12){
          lcd.setCursor(6,1);
          lcd.print("P");
          lcd.setCursor(13,1);
          lcd.print("M");
          if (hour2 != 12) { hour2 = hour2-12; }
        }
        else {
          lcd.setCursor(6,1);
          lcd.print("A");
          lcd.setCursor(13,1);
          lcd.print("M");
        }


        hr = hour2;
        
        if(hr != ohr) {
          printNum3(hr/10, 0, 0);
          printNum3(hr%10, 3, 0);
          ohr = hr;
        }
        
        if(mn != omn) {
          printNum3(mn/10, 7, 0);
          printNum3(mn%10,10, 0);
          omn = mn;
        }
        
        if(se != osec) {
          printNum3(se/10, 14, 0);
          printNum3(se%10, 17, 0);
          osec = se;
        }
        
      }
      if (gps.date.isValid()){
        day = gps.date.day();
        if (Date) {
          day = day - 1;
          Date = false;
        }
        
        
        if (day != oday || gps.date.month() != omonth || gps.date.year() != oyear){ 

          lcd.setCursor(0, 3); 
          lcd.print("                    ");
          lcd.setCursor(0,3);
          lcd.print(gps.date.month());
          lcd.print(F("/"));
          lcd.print(day);
          lcd.print(F("/"));
          lcd.print(gps.date.year());
          omonth = gps.date.month();
          oday = day;
          oyear = gps.date.year();
        }
      }
    }
  }
  status = pressure.startTemperature();
  if (currentMillis - tempMillis >= 1100) {
    tempMillis = millis();
    if (status != 0){
      // Wait for the measurement to complete:
      delay(status);
  
      // Retrieve the completed temperature measurement:
      // Note that the measurement is stored in the variable T.
      // Function returns 1 if successful, 0 if failure.
      status = pressure.getTemperature(T);
      if (status != 0)
      {
        F = (9.0/5.0)*T+32.0;
        String TempLength = String(F,1);
        lcd.setCursor(18-TempLength.length(),3);
        lcd.print(F-4,1);
        lcd.print((char)223);
        lcd.print("F");  
      }
    }
  }

}


// ********************************************************************************** //
//                                      SUBROUTINES
// ********************************************************************************** //
void printNum3(byte digit, byte leftAdjust, byte topAdjust) {
   for(row=0; row<3; row++) {
     lcd.setCursor(leftAdjust,row+topAdjust);               
     for(byte num=digit*3; num <digit*3+3; num++) {
       lcd.write(pgm_read_byte( &bn3[row][num]) );
     }
   }
}

void printColon(byte leftAdjust, byte topAdjust) {
   for(row=0; row<(4-topAdjust); row++) {
     lcd.setCursor(leftAdjust,row+topAdjust); 
     if(topAdjust == 0) {     
       if(row == 0 || row == 1) lcd.print(F(".")); else lcd.print(F(" "));
     } else {
       if(row == 1 || row == 2) lcd.print(F(".")); else lcd.print(F(" "));
     }
  }
}

