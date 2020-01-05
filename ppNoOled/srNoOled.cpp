#include <Arduino.h>
#include <U8g2lib.h>

#include "hashDefines.h"
#include "srNoOled.h"

//#ifdef OLED


extern U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2;//(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
extern byte beat;
extern byte bar;
extern byte pattern[][NBARS];
extern char bbc[];
extern int  bpm;
extern byte gv;

void u8g2_prepare(void) {
  //u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFont(u8g2_font_5x8_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}


void draw_voice(byte gv, byte col){

  u8g2.setDrawColor(col);
  u8g2.drawLine(PATT_X-5, PATT_Y+(gv*PBLOCK_GAP)+1, PATT_X-3, PATT_Y+(gv*PBLOCK_GAP)+1);
  u8g2.drawLine(PATT_X-4, PATT_Y+(gv*PBLOCK_GAP)  , PATT_X-4, PATT_Y+(gv*PBLOCK_GAP)+2);
  
  u8g2.setDrawColor(1);
}

void update_voice(byte oldgv, byte newgv){
  
  //u8g2.drawFrame(PATT_X+(3*((0-1)+(8*(2-1)))),PATT_Y,3,23);
  
  draw_voice(oldgv,0);
  draw_voice(newgv, 1);
}


void draw(void) {
  //bounding box
  u8g2.drawFrame(PATT_X-2,0,TEXT_X-PATT_X+1,32);//127,32);

  byte bb= beat;
  if(!beat)bb=8;
  u8g2.drawFrame(PATT_X+(PBLOCK_GAP*((bb-1)+(8*(bar)))),PATT_Y,PBLOCK_GAP,23);

  //Draw the current pattern
  for(byte brno=0;brno<NBARS;brno++){
    for(byte btno=0;btno<8;btno++){
      for(byte vv=0;vv<NVOICES;vv++){
        if(trigger(pattern[vv][brno],btno)){
          u8g2.drawBox(PATT_X+(PBLOCK_GAP*(btno+(8*(brno)))),PATT_Y+(PBLOCK_GAP*vv),PBLOCK_SIZE,PBLOCK_SIZE);
        }
      }
    }
  }
  
  //Draw the current position

  //Print the bar count
  sprintf(bbc,"%d/%d",bar,bb);
  u8g2.drawStr(TEXT_X,22,bbc);
  
  sprintf(bbc,"%d",bpm);
  u8g2.drawStr(TEXT_X,14,bbc);

  //draw on the current voice marker
  draw_voice(gv,1);
  
}

//#endif
