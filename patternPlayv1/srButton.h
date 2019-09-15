

#define DEBOUNCE_DELAY (100) //millisecond delay

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

