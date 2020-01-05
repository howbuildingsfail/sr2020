#ifndef SRNOOLED_H
#define SRNOOLED_H

//#ifdef NOOLED


//POSITIONS OF GRAPHIX
#define PATT_X (10)
#define PATT_Y (2)
#define TEXT_X (92)
#define PBLOCK_SIZE (4)
#define PBLOCK_GAP (PBLOCK_SIZE+1)

void update_voice(byte oldgv, byte newgv);
void u8g2_prepare(void);
void draw(void);

//#endif //nooled hashdef

#endif
