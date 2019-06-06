

#define NBARS 2
#define NVOICES 2

#define BDPIN 3
#define SDPIN 4

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
byte beatmillis = 400;
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

void setup() {
  // put your setup code here, to run once:
  bdc = bd1;
  sdc = sd1;
  
  pinMode(SDPIN,OUTPUT);
  pinMode(BDPIN,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(9600);

}


int trigger(byte pattern, byte beat) {

  //byte pand = pattern & idx[beat];
  //sp("beat = ");sp(beat,DEC);sp(", index = ");sp(idx[beat],BIN);sp(", pand = ");sp(pand,BIN);spl();

  return (((pattern & idx[beat]) << beat) >> 7);
}


long mc = 0;

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

  //Timer
  m1 += beatmillis-TRIG_LEN;
  do{
    m2 = millis(); 
  }while (m1 > m2);

}
