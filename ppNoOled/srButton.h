
#ifndef   SRBUTTON_H
#define   SRBUTTON_H

//TODO: make this value variable, so we can vary it by beat length - 
#define DEBOUNCE_DELAY (150) //millisecond delay

class srButton{

  public:
    srButton(byte _pin);

    bool check(long tt);

  private:
    byte pin;
    byte btnval;
    long debounce_time;
    long pattern_db_time;

};

#endif
