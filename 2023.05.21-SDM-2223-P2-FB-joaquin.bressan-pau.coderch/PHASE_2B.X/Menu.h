#ifndef MENU_H
#define    MENU_H

#include <xc.h> // include processor files - each processor file is guarded.
#include "LcTLCD.h"
#include "Keypad.h"
#include "Timer.h"
#include "Joystick.h"
#include "SystemTimer.h"
#include "Audio.h"
#include "SIO.h"
#include "EEPROM.h"

void initMenu(void);
void menuLoop(void);
void resetDisplay(char stg);
void moveOneRow(void);


void lcdAddString(char* string);
void displayMenu(char* string);
void refreshDisplay(void);
void goToWaitState(char stg);
#endif