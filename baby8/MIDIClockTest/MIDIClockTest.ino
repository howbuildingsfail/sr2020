
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

long int t1,now;


void handleClock(void){

  t1 = millis();
    digitalWrite(LED_BUILTIN,HIGH);

  
}


void setup() {
  // put your setup code here, to run once:

    MIDI.setHandleClock(handleClock);


    // Initiate MIDI communications, listen to all channels
    MIDI.begin(MIDI_CHANNEL_OMNI);
    Serial.begin(115200);  

    pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {

  MIDI.read();
  
  // put your main code here, to run repeatedly:
  now = millis();

  if( now> (t1+50))
    digitalWrite(LED_BUILTIN,LOW);

}
