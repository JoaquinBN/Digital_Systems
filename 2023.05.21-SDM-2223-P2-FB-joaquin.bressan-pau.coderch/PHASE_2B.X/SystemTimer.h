/* 
 * File:   SystemTimer.h
 * Author: joaqu
 *
 * Created on 16 May 2023, 15:22
 */

#ifndef SYSTEMTIMER_H
#define	SYSTEMTIMER_H

#include <xc.h>
#include "Timer.h"
#include "LcTLCD.h"

typedef struct{
    char minutes;
    char seconds;
}Timestamp;

void initSysTimer(void);
void sysTimerLoop(void);
void updateSysTimestamp(Timestamp time, char i);
void incrementSysTimestamp(void);
void displayTimestamp(char i);
char finishedDisplay(void);
Timestamp getSysTimestamp(void);

char timestampConversion(char u, char d);


#endif	/* SYSTEMTIMER_H */

