

#define SPIN 3



String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

#define NB 4    //Number of beats
#define PERIOD 1200 //might be better if this is a power of 2!
int beats[NB];
int len[] = {1,5,10,50};
int bidx=0;
int bb;


void setup() {
  pinMode(SPIN,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  for( bb=0;bb<NB;bb++){
    beats[bb]=0;
  }
}


long it = 0;
long ms=700;
char ibuffer[200];


float bpm2millis = 1000/60;

void loop() {

  long m2,m1=millis();//micros();
  
  // put your main code here, to run repeatedly:
  digitalWrite(SPIN,HIGH);
  digitalWrite(LED_BUILTIN,HIGH);
  
  //Pause for half the beat
  do{
    m2 = millis();//micros();
  }while((m2-m1)< (ms*0.5));
  
  digitalWrite(SPIN,LOW);
  digitalWrite(LED_BUILTIN,LOW);
  //delay(ms-1);

  
//  digitalWrite(SPIN,HIGH);
//  digitalWrite(LED_BUILTIN,HIGH);
//  //Need to experiment with how long a pin needs to be high to get a full latch
//  delay(3);
//  
//  digitalWrite(SPIN,LOW);
//  digitalWrite(LED_BUILTIN,LOW);
//  delay(ms/2);

//  for(int cc=0;cc<NB;cc++){
//    if(beats[cc]){
//            
//    }
//  }

  if (stringComplete) {
    Serial.println("Hello from stringComplete");
    Serial.println(inputString);
    inputString.toCharArray(ibuffer,200);
    ms= atol(ibuffer);
    beats[bb++] = ms;
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  
  Serial.print("Execution time (ms): ");Serial.print((m2-m1),DEC);
  Serial.print("; Loop ");Serial.print(it++,DEC);Serial.print(" ms = ");Serial.print(ms,DEC);Serial.println("");

  
  //Pause for half the beat
  do{
    m2 = millis();//micros();
  }while((m2-m1)<ms);  

}



/* https://www.arduino.cc/en/Tutorial/SerialEvent
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.

  Note - you need to select 'newline' from the menu at the bottom of serial monitor for this to work. 
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    
    char inChar = (char)Serial.read();
    //Serial.println(String(inChar));
    // add it to the inputString:
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
