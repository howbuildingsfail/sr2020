
#define TEMPOPOT (A1)
#define GATEPOT (A2)
#define MODEPOT (A3)
#define MODERATE (A4)

#define CLOCKINPIN (10)
#define CLOCKOUTPIN (12)


int gateout = 13;

int ledPins[] = {
  0,1,2,3,4,5,6,7
};   
#define NV (8)

/* TEMPO pot is on pin A2
 *  
 * == IDEAS ==
 * GATE time pot, to set gate time as a proportion of step time
 * NSTEPS pot - go down from 8 to 1 step (bit dull, could do in mode pot?)
 * 8/4 switch - switch between 1 cv for 8 steps or 2 cvs for 4 steps
 * MODE pot - ideas include: 
 *    1: random seq; 
 *    2: reverse seq; 
 *    3: euclidean seq; 
 *    4: syncopation; 
 *    5: 1-4*7 then 5-8;.
 * PWM out on digital pins - might be an interesting way to build a weird LFO - would need rate, depth and shape switching
 * 
 */


 

// See http://doitwireless.com/2014/06/26/8-bit-pseudo-random-number-generator/
uint8_t rnd()
{
  static uint8_t r = 0x23;
  uint8_t lsb = r & 1;
  r >>= 1;
  r ^= (-lsb) & 0xB8;
  return r;
}

uint8_t nextLED(uint8_t lastled, uint8_t mode){

  uint8_t led;
  switch(mode){
    case 1: //Random
      led = rnd()>>5;
      break;
      
    case 2: //Reverse
      if(lastled == 0)
        led = NV-1;
      else
        led = lastled-1; 
      break;

    default: //0 and any number above the modes...
      if(lastled == NV-1)
        led=0;
      else
        led=lastled+1;
    
  }
  return led;
}


void setup() {
  // put your setup code here, to run once:
  for(int ll=0;ll<NV;ll++)pinMode(ledPins[ll], OUTPUT);
  pinMode(gateout,OUTPUT);
  pinMode(CLOCKINPIN,INPUT);
  pinMode(CLOCKOUTPIN,OUTPUT);

}


const float div1023 = 1.0/1023.0;

void loop() {
  static long t2,tgate,t1 = millis();
  static int tstep,gw,gstep;
  static char led = 3;
  bool gcheck;
  static bool inbeat;

  unsigned char mode=0;

  tstep = 10 + (2*analogRead(TEMPOPOT));
  gw = analogRead(GATEPOT);

  mode = analogRead(MODEPOT)>>7;

  tgate=t2=millis();

  //Close the gate
  gstep = tstep*((float)gw/1023);
  if((tgate-t1)>gstep){//(tstep/2)){
    digitalWrite(gateout, LOW);
  }

  //Increment the sequence
  if((t2-t1)>=tstep){
    gcheck=true;
    digitalWrite(gateout, HIGH);
    
    t1=t2;
    digitalWrite(ledPins[led], LOW);

    led = nextLED(led,mode);
    
    digitalWrite(ledPins[led], HIGH);   
  }

}
