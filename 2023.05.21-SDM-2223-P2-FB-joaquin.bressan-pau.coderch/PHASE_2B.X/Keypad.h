#ifndef KEYPAD_H
#define    KEYPAD_H

#include <xc.h> // include processor files - each processor file is guarded.
#include "Timer.h"
#include "Audio.h"



void initKeypad(void);
void smsLoop(void);
void keypadLoop(void);
char getFullValue(char generic);
int getColumn (void);
char getDigit(void);
char isPressed(void);
char getCharacter(void);
char isNextKey(void);
char getDigitValue(void);

#endif    /* XC_HEADER_TEMPLATE_H */