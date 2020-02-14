#include <Arduino.h>


#include "hashDefines.h"


//////////////////////////////////////////////////////////////////////////
// MIDI libraries etc if needed:

//#define DOMIDI
#ifdef DOMIDI

#include <MIDI.h>

#define BDMIDI 36
#define SDMIDI 38
#define HHMIDI 42
#define LTMIDI 45

// Created and bind the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

#endif

//////////////////////////////////////////////////////////////////////////
// OLED libraries etc if needed:

//#define OLED
#ifdef OLED

#include <U8g2lib.h>

#include "srNoOled.h"

//OLED 192x32 stuff - probably I2C or something like that:
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED

#endif


/////////////////////////////////////////////////////////////////////////


#include "srButton.h"


#define BTN_VOICE (7)
#define BTN_HIT (8)
#define BTN_STSTOP (9)
#define BTN_SLOWER (10)
#define BTN_FASTER (11)
#define BTN_REC (12) //not needed!


srButton btn_voice(BTN_VOICE);
srButton btn_hit(BTN_HIT);
srButton btn_ststop(BTN_STSTOP);
srButton btn_slower(BTN_SLOWER);
srButton btn_faster(BTN_FASTER);

bool playing = true;


#define sp  Serial.print
#define spl Serial.println

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
int bpm = 240;
int beatmillis =  60000/bpm;  //500;//1000;//400;
int t1, t2;

//buffer for writing stuff
char bbc[8];

//Let's use eight bytes just to keep things clear!
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


byte gv = 3;



// -----------------------------------------------------------------------------
#ifdef DOMIDI

// This function will be automatically called when a NoteOn is received.
// It must be a void-returning function with the correct parameters,
// see documentation here:
// http://arduinomidilib.fortyseveneffects.com/a00022.html

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
    // Do whatever you want when a note is pressed.

    // Try to keep your callbacks short (no delays ect)
    // otherwise it would slow down the loop() and have a bad impact
    // on real-time performance.

    switch(pitch){
      case BDMIDI:
        digitalWrite(BDPIN,HIGH);
        break;
      case SDMIDI:
        digitalWrite(SDPIN,HIGH);
        break;
      case HHMIDI:
        digitalWrite(HHPIN,HIGH);
        break;
      case LTMIDI:
        digitalWrite(LTPIN,HIGH);
        break;
      default:
        digitalWrite(BDPIN,HIGH);
        break;
    }

    digitalWrite(LED_BUILTIN, HIGH);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
    // Do something when the note is released.
    // Note that NoteOn messages with 0 velocity are interpreted as NoteOffs.
    switch(pitch){
      case BDMIDI:
        digitalWrite(BDPIN,LOW);
        break;
      case SDMIDI:
        digitalWrite(SDPIN,LOW);
        break;
      case HHMIDI:
        digitalWrite(HHPIN,LOW);
        break;
      case LTMIDI:
        digitalWrite(LTPIN,LOW);
        break;
      default:
        digitalWrite(BDPIN,LOW);
        break;
    }

    //digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
}

#endif
// -----------------------------------------------------------------------------










void setup() {
  // put your setup code here, to run once:

  memset(memory,0,NMEMS*NVOICES*NBARS*sizeof(byte));

  //byte bd1    = B11011101;
  //byte bd2    =           B10110111;
  //byte sd1    = B00100010;
  //byte sd2    =           B01001011;

  //TODO: get pattern to be a pointer into the larger memory array (possibly on loading from EEPROM)
  //pattern = &(memory + 0);
 /* 
  pattern[0][0] = B11011101;
  pattern[0][1] =           B10110111;
  pattern[1][0] = B00100010;
  pattern[1][1] =           B01001011;
  pattern[2][0] = 0;
  pattern[2][1] =           B10011001;
  pattern[3][0] = B01010101;
  pattern[3][1] = 0;
*/


  pattern[0][0] = B11011101;
  pattern[0][1] =           0;
  pattern[1][0] = 0;
  pattern[1][1] =           B11011101;
  pattern[2][0] = B00100010;
  pattern[2][1] =           B00100010;
  pattern[3][0] = 0;
  pattern[3][1] =           B00000011;
  


  pinMode(VOLCAPIN,OUTPUT);
  
  pinMode(SDPIN,OUTPUT);
  pinMode(BDPIN,OUTPUT);
  pinMode(HHPIN,OUTPUT);
  pinMode(LTPIN,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //Button shizz
  
  //configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(13, OUTPUT);

  //buttons
  pinMode(BTN_VOICE, INPUT_PULLUP);
  pinMode(BTN_HIT, INPUT_PULLUP);
  pinMode(BTN_STSTOP, INPUT_PULLUP);
  pinMode(BTN_SLOWER, INPUT_PULLUP);
  pinMode(BTN_FASTER, INPUT_PULLUP);

  
#ifdef DOMIDI


  // Connect the handleNoteOn function to the library,
  // so it is called upon reception of a NoteOn.
  MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function

  // Do the same for NoteOffs
  MIDI.setHandleNoteOff(handleNoteOff);
  
  MIDI.begin(1);  // Listen to all incoming messages

  Serial.begin(115200);  
#else

  Serial.begin(9600);

#endif  


#ifdef OLED
  u8g2.begin();
  
  u8g2_prepare();
  update_voice(gv,update_gv());
#endif


}


/* TODO: this is a complicated way of updating the voice - but it's handy to return the 
 *  value of gv for the update_voice function
 *  
 */
byte update_gv(){

      gv++;
      if(gv>3)gv=0;

      return gv;
  
}



int trigger(byte pattern, byte beat) {

  //byte pand = pattern & idx[beat];
  //sp("beat = ");sp(beat,DEC);sp(", index = ");sp(idx[beat],BIN);sp(", pand = ");sp(pand,BIN);spl();

  return (((pattern & idx[beat]) << beat) >> 7);
}











long mc = 0;
byte vv;





/* Instead of waiting for the 'beat' to end within a single iteration of loop, let's 
 * cycle around the whole thing but use TimedAction objects to handle the triggering
 */
#ifdef DOMIDI 
void loop(){


  MIDI.read();

  
}


#else
//////////////////////////////////////////////////////////////////////////////////////
// Delete what's below when you have the loop() above working properly

byte gvb;
byte gvbar;

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

  
    gvb = beat;
    gvbar = bar;


    
    for(vv=0;vv<NVOICES;vv++){
      tval[vv] = trigger(pattern[vv][bar],beat);
    }
  
    for(vv=0;vv<NVOICES;vv++)
      digitalWrite(vpin[vv], tval[vv]);

    digitalWrite(VOLCAPIN,HIGH);  
    do{
      m2 = millis();
      mc++;
    }while(m1+TRIG_LEN > m2);

    digitalWrite(VOLCAPIN,LOW);  

    //Should do it this way when all the voices have the same trigger length:
    //for(vv=0;vv<NVOICES;vv++)
    //  digitalWrite(vpin[vv], LOW);
    //But for now we are just triggering the two twin-t drums on BDPIN and SDPIN
    digitalWrite(BDPIN, LOW);
    digitalWrite(SDPIN, LOW);
    digitalWrite(HHPIN, LOW);
    digitalWrite(LTPIN, LOW);

    //do{
    //  m2 = millis();
    //  mc++;
    //}while(m1+TRIG_LEN+TRIG_LEN+TRIG_LEN > m2);
    //digitalWrite(LTPIN, LOW);

  
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
  sprintf(bbc,"%d",m1-m2);


#ifdef OLED
  //update display  
  u8g2.clearBuffer();
  draw();
  u8g2.drawStr(TEXT_X,6,bbc);
  u8g2.sendBuffer();
#endif

  //update controls
  do{
    m2 = millis();

    if(btn_ststop.check(millis())){
      digitalWrite(LED_BUILTIN, HIGH);
      playing = !playing;
      //if(playing) playing = false;
      //else playing = true;
    }
    else{
      digitalWrite(LED_BUILTIN, LOW);
    }
    
    if(btn_voice.check(millis())){
      digitalWrite(LED_BUILTIN, HIGH);
#ifdef OLED
      update_voice(gv,update_gv());
#else
      update_gv();      
#endif
    }
    else{
      digitalWrite(LED_BUILTIN, LOW);
    }

    if(btn_hit.check(millis())){
      digitalWrite(LED_BUILTIN, HIGH);
      //get the bit state...

      pattern[gv][gvbar] = pattern[gv][gvbar] ^ idx[gvb];
      
    }
    else{
      digitalWrite(LED_BUILTIN, LOW);
    }

    if(btn_slower.check(millis())){
      digitalWrite(LED_BUILTIN, HIGH);
      if(bpm > 8)bpm -= 8;
      beatmillis =  60000/bpm;
    }
    else{
      digitalWrite(LED_BUILTIN, LOW);
    }

    if(btn_faster.check(millis())){
      digitalWrite(LED_BUILTIN, HIGH);
      bpm += 8;
      beatmillis = 60000/bpm;
    }
    else{
      digitalWrite(LED_BUILTIN, LOW);
    }

#ifdef OLED
    u8g2.sendBuffer(); 
#endif

  }while (m1 > m2);

}

#endif
