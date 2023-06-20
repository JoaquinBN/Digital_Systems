#include <xc.h>
#include "EEPROM.h"

#define INDEX_POSITION 0x10
#define MINUTES_POSITION 0x20
#define SECONDS_POSITION 0x30

static char state;
static char indexTemp;
static char indexEEPROM;
static char data;
static Recording recordings[8];
void initMemory(void){
    state = 0;
    indexTemp = 0;
    indexEEPROM = 0;
}

void memoryLoop(void){
    switch(state){
        //IDLE MODE
        case 0:
            break;
            
        //SAVE TIMESTAMP IN EEPROM
        case 2:
            recordings[indexEEPROM%8].timestamp = getSysTimestamp();
            if(EECON1bits.WR == 0){
                saveData(MINUTES_POSITION + indexEEPROM%8, recordings[indexEEPROM%8].timestamp.minutes);
                state = 3;
            }
            break;
        case 3:
            if(EECON1bits.WR == 0){
                saveData(SECONDS_POSITION + indexEEPROM%8, recordings[indexEEPROM%8].timestamp.seconds);
                state = 4;
            }
            break;
        case 4:
            if(checkTX( )){
                setTX(recordings[indexEEPROM%8].timestamp.minutes);
                state = 5;
            }
            break;
        case 5:
            if(checkTX()){
                setTX(recordings[indexEEPROM%8].timestamp.seconds);
                state = 0;
            }
            break;
            
       //SAVE INDEX TO EEPROM
        case 10:
            if(checkRX()){
                recordings[indexEEPROM%8].index = getRX();
                state = 11;
            }
            break;
        case 11:
            if(EECON1bits.WR == 0){
                saveData(INDEX_POSITION + indexEEPROM%8, recordings[indexEEPROM%8].index);
                indexEEPROM++;
                state = 12;
            }
            break;
        case 12:
            if(EECON1bits.WR == 0){
                saveData(0, (indexEEPROM > 8? 8 : indexEEPROM));
                state = 13;
            }
            break;
        case 13:
            if(EECON1bits.WR == 0){
                if(indexEEPROM > 8){
                    saveData(1, indexEEPROM%8 + 8);
                }else{
                    saveData(1,indexEEPROM%8);
                }
                
                state = 0;
            }
            break;
            
        case 30:
            if(recordings[indexTemp].index/10 == 0){
                state = 31;
            } else if(checkTX()){
                setTX((recordings[indexTemp].index/10) + '0');
                state = 31;
            }
            break;
        case 31:
            if(checkTX()){
                setTX((recordings[indexTemp].index%10) + '0');
                state = 32;
            }
            break;
        case 32:
            if(checkTX()){
                setTX('\0');
                state = 0;
            }
            break; 
            
        //READ NON-VOLATILE MEMORY
        case 40:
            if(EECON1bits.RD == 0){
                indexTemp = readData(0);
                state = indexTemp == 0xFF? 0:41;
            }
            break;
        case 41:
            if(EECON1bits.RD == 0){
                recordings[indexEEPROM].index = readData(INDEX_POSITION + indexEEPROM);
                state = 42;
            }
            break;
        case 42:
            if(EECON1bits.RD == 0){
                recordings[indexEEPROM].timestamp.minutes = readData(MINUTES_POSITION + indexEEPROM);
                state = 43;
            }
            break;
        case 43:
            if(EECON1bits.RD == 0){
                recordings[indexEEPROM].timestamp.seconds = readData(SECONDS_POSITION + indexEEPROM);
                indexEEPROM++;
                state = 44;
            }
            break;
        case 44:
            if(indexEEPROM == indexTemp){
                state = 45;
            }else{
                state = 41;
            }
            break;
        case 45:
            if(EECON1bits.RD == 0){
                indexEEPROM = readData(1);
                state = 0;
            }
            break;
            
            
        //DISPLAY OF RECORDINGS BY INDEX AND TIMESTAMP
        case 50:
            LcPutChar('I');
            if(recordings[indexTemp].index/10 == 0){
                state = 52;
            }else{
                state = 51;
            }
            break;
        case 51:
            LcPutChar((recordings[indexTemp].index/10) + '0');
            state = 52;
            break;
        case 52:
            LcPutChar((recordings[indexTemp].index%10) + '0');
            state = 53;
            break;
        case 53:
            LcPutChar(' ');
            updateSysTimestamp(recordings[indexTemp].timestamp,indexTemp + 1);
            state = 54;
            break;
        case 54:
            LcPutChar('-');
            state = 55;
            break;
        case 55:
            LcPutChar(' ');
            state = 56;
            break;
        case 56:
            displayTimestamp(indexTemp + 1);
            state = 57;
            break;
        case 57:
            if(finishedDisplay() && !data && indexTemp != (indexEEPROM > 8? 7 : indexEEPROM - 1)){
                data = 1;
                state = 58;
                indexTemp++;
            }else if(finishedDisplay()){
                data = 0;
                state = 0;
            }
            break;
        case 58:
            LcCursorOn();
            state = 59;
            break;
        case 59:
            LcGotoXY(0,1);
            state = 60;
            break;
        case 60:
            LcCursorOff();
            state = 50;
            break;
    }
}

//EEPROM writing sequence found in the PIC18F4321 datasheet
void saveData(char address, char data){
    EEADR = address;
    EEDATA = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    INTCONbits.GIE = 1;
    EECON1bits.WREN = 0;
}

//EEPROM reading sequence found in the PIC18F4321 datasheet
char readData(char address){
    EEADR = address;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.RD = 1;  
    return EEDATA;
}

void displayRecordings(char indexPosition){
    indexTemp = indexPosition;
    data = 0;
    state = 50;
}

char isIdle(void){
    return state == 0;
}

void sendTimestamp(void){
    state = 2;
}

void saveIndex(void){
    state = 10;
}

void sendIndex(char indexPosition){
    indexTemp = indexPosition;
    state = 30;
}

char getNumRecordings(void){
    return (indexEEPROM > 8? 8 : indexEEPROM);
}

void chargeMemory(void){
    indexEEPROM = 0;
    state = 40;
}