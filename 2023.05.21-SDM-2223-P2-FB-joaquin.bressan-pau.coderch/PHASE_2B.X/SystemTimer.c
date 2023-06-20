
#include <xc.h>
#include "SystemTimer.h"


static char sysTmr;
static char state;
static char index; // to use when using the other timestamps
static Timestamp timestamp[9];


void initSysTimer(void){
    TI_NewTimer(&sysTmr);
    timestamp[0].minutes = 0;
    timestamp[0].seconds = 0;
    state = 0;
}

void sysTimerLoop(void){
    switch(state){
        case 0:
            if(TI_GetTics(sysTmr) >= TI_SECOND){
                TI_ResetTics(sysTmr);
                incrementSysTimestamp();
            }
            break;
        case 8:
            state = 9;
            break;
        case 9:
            state = 10;
            break;
        case 10:
            LcPutChar((timestamp[index].minutes/10) + '0');
            state = 11;
            break;
            
        case 11:
            LcPutChar((timestamp[index].minutes%10) + '0');
            state = 12;
            break;
        case 12:
            LcPutChar(':');
            state = 13;
            break;
        case 13:
            LcPutChar((timestamp[index].seconds/10) + '0');
            state = 14;
            break;
        case 14:
            LcPutChar((timestamp[index].seconds%10) + '0');
            state = 0;
            break;

    }
}

//used for reset and update, can also be used for the storing of record times, adding index as param
void updateSysTimestamp(Timestamp time, char i){
    index = i;
    timestamp[index].minutes = time.minutes;
    timestamp[index].seconds = time.seconds;
    TI_ResetTics(sysTmr);
}

void incrementSysTimestamp(void){
    if(timestamp[0].seconds == 59){
        timestamp[0].seconds = 0;
        timestamp[0].minutes++;
    }else{
        timestamp[0].seconds++;
    }    
}

void displayTimestamp(char i){
    index = i;
    state = 8;
}

Timestamp getSysTimestamp(void){
    return timestamp[0];
}

char timestampConversion(char u, char d){
    return (d*10 + u)-16;
}


char finishedDisplay(void){
    return state == 0;
}