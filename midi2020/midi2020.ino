/*
 * This version reads midi to control the timings - handy for debugging the audio parts of  the circuit
 */


#include <Arduino.h>

#include "srButton.h"

#define NBARS (2)
#define NVOICES (4)

#define BDPIN (3)
#define SDPIN (4)
#define HHPIN (5)
#define LTPIN (6)

#define BTN_VOICE (7)
#define BTN_HIT (8)
#define BTN_REC (9)
#define BTN_STSTOP (10)

srButton btn_voice(BTN_VOICE);
srButton btn_hit(BTN_HIT);
srButton btn_ststop(BTN_STSTOP);

bool playing = true;

#define TRIG_LEN (15)

#define NMEMS (18)

//we don't use this yet...but let's have it now so we can check we've enough memory!
byte memory[NMEMS][NVOICES][NBARS];


//TODO: When we start to store patterns
byte pattern[NVOICES][NBARS];

//byte bd1 = B11011101;
//byte bd2 =           B10110111;
//byte sd1 = B00100010;
//byte sd2 =           B01001011;

//byte bdc;
//byte sdc;
byte beat = 0;
byte bar = 1; // has to be the last bar so we do the switch correctly
int bpm = 480;
int beatmillis =  60000/bpm;  //500;//1000;//400;
int t1, t2;

//buffer for writing stuff
char bbc[8];

//This is used to index the beat in the byte - there are potentially more compact ways of doing this!
byte idx[] = {B10000000,
              B01000000,
              B00100000,
              B00010000,
              B00001000,
              B00000100,
              B00000010,
              B00000001
             };

//int bdval, sdval;
byte tval[NVOICES];
byte vpin[] = {BDPIN,SDPIN,HHPIN,LTPIN};


unsigned long m1, m2;

    



void setup() {
  // put your setup code here, to run once:

  memset(memory,0,NMEMS*NVOICES*NBARS*sizeof(byte));

  //byte bd1    = B11011101;
  //byte bd2    =           B10110111;
  //byte sd1    = B00100010;
  //byte sd2    =           B01001011;

  //TODO: get pattern to be a pointer into the larger memory array (possibly on loading from EEPROM)
  //pattern = &(memory + 0);
  
  pattern[0][0] = B11011101;
  pattern[0][1] =           B10110111;
  pattern[1][0] = B00100010;
  pattern[1][1] =           B01001011;
  pattern[2][0] = 0;
  pattern[2][1] =           B10011001;
  pattern[3][0] = B01010101;
  pattern[3][1] = 0;
  
  pinMode(SDPIN,OUTPUT);
  pinMode(BDPIN,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //Button shizz
  
  //configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(13, OUTPUT);

  //buttons
  pinMode(BTN_VOICE, INPUT_PULLUP);
  pinMode(BTN_HIT, INPUT_PULLUP);
  pinMode(BTN_STSTOP, INPUT_PULLUP);

  
  Serial.begin(9600);
}







int trigger(byte pattern, byte beat) {

  //byte pand = pattern & idx[beat];
  //sp("beat = ");sp(beat,DEC);sp(", index = ");sp(idx[beat],BIN);sp(", pand = ");sp(pand,BIN);spl();

  return (((pattern & idx[beat]) << beat) >> 7);
}





long mc = 0;
byte vv;














void loop() {
  mc = 0;
  m1 = millis();
  
  if(playing){
    //cycle through the bars..
    if (!beat) {
      switch (bar) {
        case 0:
          bar = 1;
          break;
        case 1:
          bar = 0;
          break;
      }
    }
    
    for(vv=0;vv<NVOICES;vv++){
      tval[vv] = trigger(pattern[vv][bar],beat);
    }
  
    for(vv=0;vv<NVOICES;vv++)
      digitalWrite(vpin[vv], tval[vv]);
      
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

  }

  //NOW WE CAN USE THE REMAINING TIME TO UPDATE THINGS..


  //Timer
  m1 += beatmillis-TRIG_LEN;
  m2 = millis();

  //update controls
  do{
    m2 = millis();

    if(btn_voice.check(m2)){
      digitalWrite(LED_BUILTIN, HIGH);
      update_voice();
    }
    else{
      digitalWrite(LED_BUILTIN, LOW);
    }

    if(btn_hit.check(m2)){
      digitalWrite(LED_BUILTIN, HIGH);
      if(playing) playing = false;
      else playing = true;
    }
    else{
      digitalWrite(LED_BUILTIN, LOW);
    }

    if(btn_ststop.check(m2)){
      digitalWrite(LED_BUILTIN, HIGH);
      if(playing) playing = false;
      else playing = true;
    }
    else{
      digitalWrite(LED_BUILTIN, LOW);
    }

  }while (m1 > m2);

}
