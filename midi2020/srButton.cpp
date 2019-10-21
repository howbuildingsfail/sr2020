
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

  return false;
}
