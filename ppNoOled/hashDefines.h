
#ifndef HASHDEFINES
#define HASHDEFINES

#define NBARS (2)
#define NVOICES (4)
#define NPATTERNS (8)

#define VOLCAPIN (2)

#define SDPIN (6)
#define BDPIN (4)
#define LTPIN (3)
#define HHPIN (5)

#define LED_SD_PIN (A0)
#define LED_BD_PIN (A1)
#define LED_BEAT_PIN (A2)
#define LED_LT_PIN (A3)
#define LED_HH_PIN (A4)

#define BTN_HIT (7)
#define BTN_STSTOP (8)
#define BTN_VOICE (9)
#define BTN_SLOWER (10)
#define BTN_FASTER (11)

//Here are the indexes in the pattern array
#define SD_IDX (0)
#define BD_IDX (1)
#define LT_IDX (2)
#define HH_IDX (3)

// Pattern structure

typedef struct tds{
  byte p[NBARS][NVOICES];
  byte bpm = 0;
} pattnstruct;


// Function definitions
int trigger(byte pattern, byte beat);

#endif
