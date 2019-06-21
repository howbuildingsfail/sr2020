
#include <Arduino.h>
#include <U8g2lib.h>

//OLED 192x32 stuff - probably I2C or something like that:
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED



#define NBARS 2
#define NVOICES 2

#define BDPIN 3
#define SDPIN 4

#define BTNPIN (5)

#define sp  Serial.print
#define spl Serial.println

#define TRIG_LEN 15

bool play[NVOICES];

byte bd1 = B11011101;
byte bd2 =           B10110111;
byte sd1 = B00100010;
byte sd2 =           B01001011;

byte bdc;
byte sdc;
byte beat = 0;
byte bar = 2; // has to be the last bar so we do the switch correctly
int bpm = 360;
int beatmillis =  60000/bpm;  //500;//1000;//400;
int t1, t2;

byte idx[] = {B10000000,
              B01000000,
              B00100000,
              B00010000,
              B00001000,
              B00000100,
              B00000010,
              B00000001
             };

int bdval, sdval;



unsigned long m1, m2;



void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}



#define PATT_X (10)
#define PATT_Y (2)



byte gv = 3;


void draw_voice(byte col){

  u8g2.setDrawColor(col);
  u8g2.drawLine(PATT_X-4, PATT_Y+(gv*4)+1, PATT_X-2, PATT_Y+(gv*4)+1);
  u8g2.drawLine(PATT_X-3, PATT_Y+(gv*4)  , PATT_X-3, PATT_Y+(gv*4)+2);
  
  u8g2.setDrawColor(1);
}



void update_voice(){

  
  //u8g2.drawFrame(PATT_X+(3*((0-1)+(8*(2-1)))),PATT_Y,3,23);
  
  draw_voice(0);
  gv++;
  if(gv>3)gv=0;
  draw_voice(1);

  
}





void draw(void) {


  u8g2.drawFrame(1,0,127,32);


  byte bb= beat;
  if(!beat)bb=8;
  u8g2.drawFrame(PATT_X+(3*((bb-1)+(8*(bar-1)))),PATT_Y,3,23);

  //Draw the current pattern
  for(byte brno=1;brno<3;brno++){
    for(byte btno=0;btno<8;btno++){
      switch(brno){
        case 1:
          
          if(trigger(bd1, btno)){
            u8g2.drawBox(PATT_X+(3*(btno+(8*(brno-1)))),PATT_Y+4,3,3);
          }
          else{
            
          }
          if(trigger(sd1, btno)){
            u8g2.drawBox(PATT_X+(3*(btno+(8*(brno-1)))),PATT_Y+8,3,3);
          }
          else{
            
          }

          break;
        case 2:
          
          if(trigger(bd2, btno)){
            //int xx = PATT_X+(3*(btno+(8*(brno-1))))+1;
            //u8g2.drawLine(xx,PATT_Y+4,xx,PATT_Y+6);
            u8g2.drawBox(PATT_X+(3*(btno+(8*(brno-1)))),PATT_Y+4,3,3);
          }
          else{
            
          }
          if(trigger(sd2, btno)){
            //int xx = PATT_X+(3*(btno+(8*(brno-1))))+1;
            //u8g2.drawLine(xx,PATT_Y+8,xx,PATT_Y+10);
            u8g2.drawBox(PATT_X+(3*(btno+(8*(brno-1)))),PATT_Y+8,3,3);
          }
          else{
            
          }

          break;
      }
    }
  }
  
  //Draw the current position

  char bbc[8];
  sprintf(bbc,"%d/%d",bar,bb);
  u8g2.drawStr(10,22,bbc);
  
  //sprintf(bbc,"bms = %d",beatmillis);
  //u8g2.drawStr(70,10,bbc);
  //sprintf(bbc,"bpm = %d",bpm);
  //u8g2.drawStr(70,18,bbc);
  
  //sprintf(bbc,"bms = %d",beatmillis);
  //u8g2.drawStr(70,10,bbc);
  sprintf(bbc,"%d",bpm);
  u8g2.drawStr(70,18,bbc);


  //draw on the current voice marker
  draw_voice(1);
  
}

















void setup() {
  // put your setup code here, to run once:
  bdc = bd1;
  sdc = sd1;
  
  pinMode(SDPIN,OUTPUT);
  pinMode(BDPIN,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //Button shizz
  
  //configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(BTNPIN, INPUT_PULLUP);
  pinMode(13, OUTPUT);


  
  Serial.begin(9600);
  
  u8g2.begin();

  u8g2_prepare();
  update_voice();
}


int trigger(byte pattern, byte beat) {

  //byte pand = pattern & idx[beat];
  //sp("beat = ");sp(beat,DEC);sp(", index = ");sp(idx[beat],BIN);sp(", pand = ");sp(pand,BIN);spl();

  return (((pattern & idx[beat]) << beat) >> 7);
}


long mc = 0;

//TODO: put these in a class
byte btnval;
long debounce_time;
#define DEBOUNCE_DELAY (100) //millisecond delay




void loop() {
  mc = 0;
  m1 = millis();
  

  //cycle through the bars..
  if (!beat) {
    switch (bar) {
      case 1:
        bar = 2;
        break;
      case 2:
        bar = 1;
        break;
    }
  }
  switch (bar) {
    case 1:
      bdval = trigger(bd1, beat);
      sdval = trigger(sd1, beat);
      break;
    case 2:
      bdval = trigger(bd2, beat);
      sdval = trigger(sd2, beat);
      break;
  }

  digitalWrite(BDPIN, bdval);
  digitalWrite(SDPIN, sdval);

  do{
    m2 = millis();
    mc++;
  }while(m1+TRIG_LEN > m2);
  
  digitalWrite(BDPIN, LOW);
  digitalWrite(SDPIN, LOW);

  //This reports to serial, but also puts a bit of noise on the line!
  //sp("Bar ");sp(bar,DEC);sp(", Beat ");sp(beat,DEC);sp(", bdval = ");sp(bdval,DEC);sp(", sdval = ");sp(sdval,DEC);sp(", mc = ");sp(mc,DEC);spl();
  
  //update beat
  beat++;
  if (beat > 7) beat = 0;

  //update display  
  u8g2.clearBuffer();
  draw();
  u8g2.sendBuffer();

  //Timer
  m1 += beatmillis-TRIG_LEN;
  do{
    m2 = millis();

    if((m2 - debounce_time)>DEBOUNCE_DELAY){
      btnval = digitalRead(BTNPIN);
      if (btnval == HIGH) {
        digitalWrite(13, LOW);
      } else {
        digitalWrite(13, HIGH);
        update_voice();
        u8g2.sendBuffer();
      }
      debounce_time = m2;
    }

  }while (m1 > m2);

}
