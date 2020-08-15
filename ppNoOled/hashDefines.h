
#ifndef HASHDEFINES
#define HASHDEFINES

#define NBARS (2)
#define NVOICES (4)
#define NPATTERNS (8)

#define VOLCAPIN (2)
#define BDPIN (3)
#define SDPIN (4)
#define HHPIN (5)
#define LTPIN (6)

#define LED_BEAT_PIN (A0)
#define LED_BD_PIN (A1)
#define LED_SD_PIN (A2)
#define LED_HH_PIN (A3)
#define LED_LT_PIN (A4)

#define BTN_VOICE (7)
#define BTN_HIT (8)
#define BTN_STSTOP (9)
#define BTN_SLOWER (10)
#define BTN_FASTER (11)
#define BTN_REC (12) //not needed!

// Pattern structure

typedef struct tds{
  byte p[NBARS][NVOICES];
  byte bpm = 0;
} pattnstruct;




// Function definitions
int trigger(byte pattern, byte beat);

#endif
