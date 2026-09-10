#ifndef GLCD_H
#define GLCD_H

#include<p30Fxxxx.h>

/* General use definitions */
#define RIGHT		0
#define LEFT		1
#define BUSY		0x80


void SetRST(unsigned char vrednost);
void SetE(unsigned char vrednost);
void SetRW(unsigned char vrednost);
void SetRS(unsigned char vrednost);
void SetCS2(unsigned char vrednost);
void SetCS1(unsigned char vrednost);
void ConfigureAllPins (void);
void ConfigureLcdData(unsigned char direction);
void SetLcdData(unsigned char vrednost);
unsigned char ReadLcdData(void);


void strobe_data(void);

void LcdInstructionWrite (unsigned char u8Instruction);
void LcdDelay(unsigned int u32Duration);
void LcdWaitBusy (void);
void GoToY(unsigned char y);
void GoToX(unsigned char x);
void GoToXY(unsigned char x,unsigned char y);
void LcdSelectStartline(unsigned char startna_linija);
void LcdSelectSide(unsigned char u8LcdSide);
unsigned char LcdDataRead (void);
void GLCD_LcdInit(void);
void LcdDataWrite (unsigned char u8Data);
void GLCD_ClrScr (void);
void GLCD_FillScr (void);
void GLCD_DisplayPicture (unsigned char *slika);
void LcdSetDot (unsigned char u8Xaxis, unsigned char u8Yaxis);
void LcdResDot (unsigned char u8Xaxis, unsigned char u8Yaxis);
void GLCD_Circle (unsigned char u8CenterX, unsigned char u8CenterY, unsigned char u8Radius);
void GLCD_Rectangle (unsigned char u8Xaxis1,unsigned char u8Yaxis1,unsigned char u8Xaxis2,unsigned char u8Yaxis2);
void Glcd_PutChar (char AskiKod);
void Glcd_PutCharBig (char AskiKod);
void GLCD_Printf (char *au8Text);
void GLCD_ShowGreed(unsigned char razmak_greeda);

#endif
