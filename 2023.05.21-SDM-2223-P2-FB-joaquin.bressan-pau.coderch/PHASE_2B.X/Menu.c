#include <xc.h>
#include "Menu.h"

static char PORTNAMEMENU[12]=           {"PORT NAME: \0"};
static const char MAINMENU[6][23] =     {"1.START RECORDING     \0"
                                        ,"2.PLAY RECORDINGS     \0"
                                        ,"3.MODIFY CURRENT TIME \0"
                                        ,"4.SHOW CURRENT TIME   \0"
                                        ,"5.END COMMUNICATION   \0"
                                        ,"                      \0"};
static char STARTRECORDING[14] =      {"RECORDING...\0"};
static char PLAYRECORDINGS[12] =      {"PLAYING...\0"};

static char MODIFYTIME[13] =      {"MODIFY TIME:\0"};
static char CURRENTTIME[14] =         {"CURRENT TIME:\0"};
static char ENDCOMMUNICATION[11]=       {"bye bye \0"};

static char ERRORMESSAGES[20] = {"NO RECORDS FOUND\0"};

static char MUSICPERIODS[8] = {3,2,3,1,4,3,2,5};
static int MUSICDELAY[8] = {1000,1000,1000,2400,1000,1000,600,1200};

static char timer;
static char songTimer;

static char rowIndex = 0; 
static char colIndex = 0;
static char displayPos;
static char index = 0;
static Timestamp timeValue;
static char portName[4];
static char skipState;
static char state = 0;
static char tmpPortChar = 0;
static int scroll = 0;
static int scrollIndex = 0;
static char waitState = 0;
static char stateToGo = 0;

void initMenu(void){
    TI_NewTimer(&timer);
    TI_NewTimer(&songTimer);
}

void menuLoop(void){
    switch (state){ 
        case 0:
            resetDisplay(1);
            skipState = 0;
            index = 0;
            timeValue.minutes = 0;
            timeValue.seconds = 0;
            displayPos = 0;
            tmpPortChar = 0;
            LcCursorOn();
            break;
        case 24:
            if(waitState == 30){
                state = stateToGo;
            }else{
                waitState++;
            }
            break;
            
        case 1:
            lcdAddString(&PORTNAMEMENU);
            goToWaitState(1);
            if(rowIndex ==1 ){
                state = 2;
            }
            break;
        case 2:
            if (colIndex >= 3 ){
                state = 0;
            }else if (isPressed()){
                if(getDigitValue() == '*'){ 
                    state = 0; //delete port name input
                }else if (getDigitValue() == '#' && colIndex == 2){ // must have pressed # and written three letters
                    tmpPortChar = 1;
                    state = 8;
                }else if (getDigitValue() != '#'){
                    if (isNextKey() || skipState == 0){
                        LcGotoXY(colIndex,rowIndex);
                        colIndex--;
                    }
                    tmpPortChar = getCharacter();
                    state = 3;
                } 
            }
            break;
            
        case 3:
            portName[getColumnAct()] = tmpPortChar;
            LcPutChar(tmpPortChar);
            colIndex++;
            skipState++;
            goToWaitState(4);
            break;
            
        case 4:
            if(!isPressed()){
                state = 2;
            }
            break;
            
        case 5:
            displayMenu(&MAINMENU[displayPos]);
           goToWaitState(5);
            state = 5;
            if(rowIndex ==1 && !skipState){
                displayPos++;
                skipState = 1;
            }else if(rowIndex == 2){
                displayPos--;
                tmpPortChar = 1;
                skipState = 5;
                state = 90;
            }
            break;
            
        case 7:
            if(displayPos == 0){
                //start recording
                state = 40;
            }else if(displayPos == 1){
                //play recording
                state = 60;
            }else if(displayPos == 2){
                //modify system time
                state = 20;
            }else if(displayPos == 3){
                //show current time
                state = 30;
            }else if (displayPos == 4){
                //end communication
                state = 100;

            }
            break;
            
        case 8:
            LcCursorOff();
            updateSysTimestamp(timeValue,0); //reset system timer
            chargeMemory();
            state = 9;
            break;
        
        //SEND TOWER NAME TO INTERFACE
        case 9:
            if(checkTX()){
                setTX('T');
                state = 10;
            }
            break;
        case 10:
            if(checkRX() && getRX() == 'K'){
                state = 11;
                index = 0;
                TI_ResetTics(timer);
            }
            break;
        case 11:
            if(index == 4){
                refreshDisplay();
            }else if(checkTX()){
                setTX(portName[index]);
                TI_ResetTics(timer);
                index++;
            }
            break;
            
        case 20:
            lcdAddString(&MODIFYTIME);
            if(rowIndex == 1){
                LcCursorOn();
                index = 4;
                state = 21;
            }
            break;
           
        case 21:
            if (isPressed()){
                if(getDigitValue() == '*'){
                    refreshDisplay();
                }else if(getDigitValue() != '#'){
                    LcPutChar(getDigitValue());
                    colIndex++;
                    index--;
                    if(index == 3){
                        timeValue.minutes = (getDigitValue()*10);
                    }else if(index == 2){
                        timeValue.minutes += getDigitValue()-16;
                    }else if(index == 1){
                        timeValue.seconds = (getDigitValue()*10);
                    }else{
                        timeValue.seconds += getDigitValue()-16;
                    }
                    state = index==0?23:22;
                } 
            }
            break;
            
        case 22:
            if(!isPressed()){
                if(index == 2){
                    LcPutChar(':');
                    colIndex++;
                }
                state = 21;
            }
            break;
        case 23:
            if(isPressed() && getCharacter() == '#'){
                updateSysTimestamp(timeValue,0);
                refreshDisplay();
            }else if(isPressed() && getCharacter() == '*'){
                refreshDisplay();
            }
            break;
        case 30:
            lcdAddString(&CURRENTTIME);
            if(rowIndex == 1){
                displayTimestamp(0);
                TI_ResetTics(timer);
                state = 31;
            }
            break;
        case 31:
            if(isPressed() && getCharacter() == '#'){
                refreshDisplay();
            }else if(TI_GetTics(timer) >= TI_SECOND){
                resetDisplay(30);
            }
            break;
            
            
         //START RECORDING CASES
        case 40:
            lcdAddString(&STARTRECORDING);
            if(rowIndex == 1){
                index = 0;
                state = 41;
            }
            break;
        case 41:
            if(checkTX()){
                setTX('D');
                state = 42;
            }
            break;
        case 42:
            sendTimestamp();
            state = 43;
            break;
        case 43:
            if(isIdle()){
                saveIndex();
                state = 44;
            }
            break;
        case 44:
            if(isIdle()){
                state = 45;
                TI_ResetTics(timer);
                joystickIdle();
                ADCON0bits.GO_DONE = 1;
            }
            break;
        case 45:
            if(TI_GetTics(timer) >= 8*TI_SECOND){
                TI_ResetTics(songTimer);
                TI_ResetTics(timer);
                joystickAlive();
                state = 50;
            }else if(ADCON0bits.GO_DONE == 0 && checkTX()){
                setTX(ADRESH);
                ADCON0bits.GO_DONE = 1;
            }
            break; 
            
        //SONG AFTER RECORDING
        case 50:
            if(TI_GetTics(songTimer) >= MUSICDELAY[index] ){
                index++;
                if(index == 8){
                    state = 51;
                }else{
                    playMusic(MUSICPERIODS[index]);
                    TI_ResetTics(songTimer);
                }
            }
            break;
            
        case 51:
            if(TI_GetTics(timer) >= 5*TI_SECOND){
                stopMusic();     
                tmpPortChar = 1;
                refreshDisplay();
            }else{
                index = 0; 
                state = 50;
            }
            break;
            
           
            
        //PLAY RECORDINGS CASES
            
        case 60:
            if(getNumRecordings() == 0){
                state = 150;
            }else{
                displayPos = 0;
                skipState = 0;
                state = 61;
            }
            break;
        case 61:
            displayRecordings(displayPos);
            state = 62;
            break; 
            
        case 62:
            if(isIdle()){
                tmpPortChar = 0;
                skipState = getNumRecordings(); 
                state = 90;
            }
            break;
            
        case 70:
            lcdAddString(&PLAYRECORDINGS);
            if(rowIndex == 1){
                state = 71;
            }
            break;
        case 71:
            if(checkTX()){
                setTX('P');
                state = 72;
            }
            break;
        case 72:
            if(checkRX() && getRX() == 'K'){
                sendIndex(displayPos);
                state = 73;
            }
            break;
        case 73:
            //RECEIVE SAMPLES
            if(isIdle()){
                state = 84;
            }
            break;
            
        case 84:
            if(checkRX() && getRX() == 'F'){
                index = 7;
                TI_ResetTics(timer);
                playMusic(index);
                state = 85;
            }
            break;
        case 85:
            if(TI_GetTics(timer) >= TI_SECOND && index != 2){
                index--;
                playMusic(index);
                TI_ResetTics(timer);
            }else if (index == 2){
                stopMusic();
                tmpPortChar = 1;
                displayPos = 1;
                refreshDisplay();
            }
            break;
            
        //JOYSTICK MOVEMENT
        case 90:
            if(!(getJoystickUp() || getJoystickDown())){
                state = 91;
                TI_ResetTics(timer);
            }
            break;
            
        case 91:
            // TAKE INTO ACCOUNT THAT NUMRECORD -1 CAN BE 4 
            if(isPressed() && getDigitValue() == '#'){
                skipState = 0;
                if(tmpPortChar){
                    resetDisplay(7);
                }else{
                    resetDisplay(70);
                }
            }else{
                if(getJoystickDown()){
                    if(displayPos < skipState - 1){
                        displayPos++;
                        refreshDisplay();
                    }
                }else if(getJoystickUp()){
                    if(displayPos > 0){
                        displayPos--;
                        refreshDisplay();
                    }
                }else if(tmpPortChar){
                    if(TI_GetTics(timer) >= TI_SECOND){
                        resetDisplay(5);
                        scroll = (scroll + 1)%22;
                        skipState = 0;
                        TI_ResetTics(timer);
                    }
                }
            }
            break;
            
        //END COMMUNICATION CASES
        case 100: 
            lcdAddString(&ENDCOMMUNICATION);
            if(rowIndex == 1){
                index = 0;
                state = 101;
            }
            break;
        case 101:
            LcCursorOn();
            state = 102;
            break;
        case 102:
            LcGotoXY(8,0);
            state = 103;
            break;
        case 103:
            LcCursorOff();
            state = 104;
            break;
                
        case 104:
            LcPutChar(portName[index]);
            index++;
            if(index == 3){
                state = 105;
            }
            break;
        case 105:
            LcPutChar('!');
            TI_ResetTics(timer);
            state = 106;
            break;
        case 106:
            if(TI_GetTics(timer) >= TI_SECOND*2){
                state = 0;
            }
            break;
            
        //ERROR CASES
        case 150:
            lcdAddString(&ERRORMESSAGES);
            if(rowIndex == 1){
                TI_ResetTics(timer);
                state = 151;
            }
            break;
        case 151:
            if(TI_GetTics(timer) >= TI_SECOND*3){
                TI_ResetTics(timer);
                refreshDisplay();
            }
            break;
    }
}

void resetDisplay(char stg){
    LcClear();
    colIndex = rowIndex = 0;
    scrollIndex = 0;
    goToWaitState(stg);
}

void lcdAddString(char* string){
    if(string[colIndex] != '\0'){
        LcPutChar(string[colIndex]);
        colIndex++;
    }else{
        moveOneRow();
    }
    
}

void displayMenu(char* string){
    if(scrollIndex < 16){
        if((scrollIndex + scroll) >= 22) {
            LcPutChar(string[(scrollIndex + scroll) - 22]);
        } else {
            LcPutChar(string[(scrollIndex + scroll)]);
        }
        colIndex++;
        scrollIndex++;
    }else{
        moveOneRow();
    }
    
}
void moveOneRow(void){
    rowIndex++;
    LcGotoXY(0,1);
    colIndex = 0;
    scrollIndex = 0;
}


void refreshDisplay(void){
    if(tmpPortChar){
        resetDisplay(5);
    }else{
        resetDisplay(61);
    }
    scroll = 0;
    skipState = 0;
    index = 0;
}

void goToWaitState(char stg){
    stateToGo = stg;
    state = 24;
    waitState = 0;
}