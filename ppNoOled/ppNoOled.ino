#include <Arduino.h>

#include "hashDefines.h"

  
//////////////////////////////////////////////////////////////////////////
// PROTOTHREADS:
#include <pt.h>

static struct pt protoThreadB; //Beat
static struct pt protoThreadC; //Controls

//////////////////////////////////////////////////////////////////////////
// MIDI libraries etc if needed:

//#define DOMIDI
#ifdef DOMIDI

#include <MIDI.h>

#define BDMIDI 36
#define SDMIDI 39
#define HHMIDI 42
#define LTMIDI 45

byte BDtoggle=0,LTtoggle=0;


// Created and bind the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

#endif
/////////////////////////////////////////////////////////////////////////


#include "srButton.h"

byte gv_led[] = {LED_SD_PIN,LED_BD_PIN,LED_LT_PIN,LED_HH_PIN};

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
pattnstruct pat[NPATTERNS];
byte pattern[NVOICES][NBARS];
//byte *pattern[NVOICES][NBARS];

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
//byte vpin[] =   {SDPIN,BDPIN,LTPIN,HHPIN};




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
        digitalWrite(BDPIN,BDtoggle = !BDtoggle);//HIGH);
        break;
      //case 39:
      case SDMIDI:
        digitalWrite(SDPIN,HIGH);
        break;
      case HHMIDI:
        digitalWrite(HHPIN,HIGH);
        break;
      case LTMIDI:
        digitalWrite(LTPIN,LTtoggle = !LTtoggle);
        break;
      default:
        //digitalWrite(BDPIN,HIGH);
        break;
    }

    digitalWrite(LED_BUILTIN, HIGH);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
    // Do something when the note is released.
    // Note that NoteOn messages with 0 velocity are interpreted as NoteOffs.
    switch(pitch){
      //case BDMIDI:
      //  digitalWrite(BDPIN,LOW);
      //  break;
      case SDMIDI:
        digitalWrite(SDPIN,LOW);
        break;
      case HHMIDI:
        digitalWrite(HHPIN,LOW);
        break;
      //case LTMIDI:
      //  digitalWrite(LTPIN,LOW);
      //  break;
      default:
        //digitalWrite(BDPIN,LOW);
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

  //pattern = (pat[0].p);
  pattern[BD_IDX][0] = B11011101;
  pattern[BD_IDX][1] =           0;
  pattern[LT_IDX][0] = 0;
  pattern[LT_IDX][1] =           B11011101;
  pattern[HH_IDX][0] = B00100010;
  pattern[HH_IDX][1] =           B00100010;
  pattern[SD_IDX][0] = 0;
  pattern[SD_IDX][1] =           B00000011;
  
  pinMode(VOLCAPIN,OUTPUT);
  
  pinMode(SDPIN,OUTPUT);
  pinMode(BDPIN,OUTPUT);
  pinMode(HHPIN,OUTPUT);
  pinMode(LTPIN,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(LED_BEAT_PIN, OUTPUT);
  pinMode(LED_BD_PIN, OUTPUT);
  pinMode(LED_SD_PIN, OUTPUT);
  pinMode(LED_HH_PIN, OUTPUT);
  pinMode(LED_LT_PIN, OUTPUT);
  //digitalWrite(LED_BD_PIN,HIGH);
  update_gv();
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
  
  MIDI.begin(1);  // Listen to messages on channel 1

  Serial.begin(115200);  
#else

  Serial.begin(9600);

#endif  

  PT_INIT(&protoThreadB);
  PT_INIT(&protoThreadC);

}//END of setup()



//TODO: this could be more efficient!
void set_voice_led(byte LED){

  digitalWrite(LED_BD_PIN, LOW);
  digitalWrite(LED_SD_PIN, LOW);
  digitalWrite(LED_HH_PIN, LOW);
  digitalWrite(LED_LT_PIN, LOW);
  
  digitalWrite(LED, HIGH);
  
}



/* TODO: this is a complicated way of updating the voice - but it's handy to return the 
 *  value of gv for the update_voice function
 *  
 */
byte update_gv(){

      gv++;
      if(gv>3)gv=0;

      set_voice_led(gv_led[gv]);

      return gv;
  
}



int trigger(byte pattern, byte beat) {

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



/* protothread for keeping da beat
 *   
 */
static int protothreadBeat(struct pt *pt)
{

  PT_BEGIN(pt);
  m1=millis();

  if(playing){
    //cycle through the bars..
    if (!beat) {
      bar = 1-bar;
      digitalWrite(LED_BEAT_PIN, HIGH);
    }
    else{
      digitalWrite(LED_BEAT_PIN, LOW);
    }
  
    gvb = beat;
    gvbar = bar;
    
    for(vv=0;vv<NVOICES;vv++){
      tval[vv] = trigger(pattern[vv][bar],beat);
    }
  
    for(vv=0;vv<NVOICES;vv++)
      digitalWrite(vpin[vv], tval[vv]);

    //Send the volca timer info
    digitalWrite(VOLCAPIN,HIGH);  
    PT_WAIT_UNTIL(pt, millis() - m1 > TRIG_LEN);
    digitalWrite(VOLCAPIN,LOW);  

    //NB! We are assuming TRIG_LEN is the same for all voices here!
    //for(vv=0;vv<NVOICES;vv++)
    //  digitalWrite(vpin[vv], LOW);
    digitalWrite(BDPIN, LOW);
    digitalWrite(SDPIN, LOW);
    digitalWrite(HHPIN, LOW);
    digitalWrite(LTPIN, LOW);

    //update beat
    beat++;
    if (beat > 7) beat = 0;
  }

  
  //PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 1000);
  //do{
  //  m2 = millis();
  //}while (m1 > m2);
  PT_WAIT_UNTIL(pt, millis() - m1 > beatmillis);
  
  PT_END(pt);
}





static int protothreadCont(struct pt *pt)
{
    PT_BEGIN(pt);


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
      update_gv();   
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
      if(bpm > 8)
        bpm -= 8;
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

    PT_END(pt);
}




void loop() {

  protothreadBeat(&protoThreadB);
  protothreadCont(&protoThreadC);
  
}

#endif
