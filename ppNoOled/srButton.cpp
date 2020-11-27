#include <Arduino.h>
#include "srButton.h"


srButton::srButton(byte _pin){
  pin = _pin;  
}



bool srButton::check(long tt){

  if((tt - debounce_time)>DEBOUNCE_DELAY){
    btnval = digitalRead(pin);
    debounce_time = tt;
    if (btnval == HIGH) {
      return false;
    } else {
      return true;
    }
  }

/* https://arduinogetstarted.com/tutorials/arduino-button-long-press-short-press
  // read the state of the switch/button:
  currentState = digitalRead(BUTTON_PIN);

  if(lastState == HIGH && currentState == LOW) {        // button is pressed
    pressedTime = millis();
    isPressing = true;
    isLongDetected = false;
  } else if(lastState == LOW && currentState == HIGH) { // button is released
    isPressing = false;
  }

  if(isPressing == true && isLongDetected == false) {
    long pressDuration = millis() - pressedTime;

    if( pressDuration > LONG_PRESS_TIME ) {
      Serial.println("A long press is detected");
      isLongDetected = true;
    }
  }

  // save the the last state
  lastState = currentState;
*/

  

  return false;
}
