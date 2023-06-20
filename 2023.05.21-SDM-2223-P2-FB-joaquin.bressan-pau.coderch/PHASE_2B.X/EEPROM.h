
#ifndef EEPROM_H
#define	EEPROM_H

#include <xc.h>
#include "LcTLCD.h"
#include "Timer.h"
#include "SystemTimer.h"
#include "Menu.h"
#include "SIO.h"

typedef struct{
    char index;
    Timestamp timestamp;
}Recording;

void initMemory(void);
void memoryLoop(void);
void saveData(char index, char data);
char readData(char index);
void displayRecordings(char indexPosition);
char isIdle(void);
void sendTimestamp(void);
void saveIndex(void);
void sendIndex(char indexPosition);
char getNumRecordings(void);
void chargeMemory(void);
#endif	/* EEPROM_H */

