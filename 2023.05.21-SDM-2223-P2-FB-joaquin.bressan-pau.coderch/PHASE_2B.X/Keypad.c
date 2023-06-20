#include <xc.h>
#include <pic18f4321.h>

#include "Keypad.h"


static const char SMSVALUES[9][6] = {"1\0","ABC2\0","DEF3\0","GHI4\0","JKL5\0","MNO6\0","PQRS7\0","TUV8\0","WXYZ9\0"};
static const char DIGITVALUES[12]= {'1','2','3','4','5','6','7','8','9','*','0','#'};

static int row;
static unsigned char debouncerTimer,SMSTimer;
static char nextKey = 0;
static char state = 0;
static char stateSMS = 0;

static char currentKey;
static char previousKey;
static char keyPressed;
static char numPressed;

void initKeypad(void){
    INTCON2bits.RBPU = 0;
  
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 1;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 1;
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;
    TI_NewTimer(&debouncerTimer);
    TI_NewTimer(&SMSTimer);
    
    //LcCursorOn();
}


void keypadLoop(void){ 
    
    switch (state){
        case 0:
       
            if (PORTBbits.RB3 && PORTBbits.RB1 && PORTBbits.RB5){
                LATBbits.LATB2=0;
                LATBbits.LATB7=1;
                LATBbits.LATB6=1;
                LATBbits.LATB4=1;
                state=1;
                row=0;
            }else{
                state=4;
            }
            break;
        case 1:
           if (PORTBbits.RB3 && PORTBbits.RB1 && PORTBbits.RB5){
                LATBbits.LATB2=1;
                LATBbits.LATB7=0;
                LATBbits.LATB6=1;
                LATBbits.LATB4=1;
                state=2;
                row=1;

            }else{
                state=4;
            }
           break;
        case 2:
            if (PORTBbits.RB3 && PORTBbits.RB1 && PORTBbits.RB5){
                LATBbits.LATB2=1;
                LATBbits.LATB7=1;
                LATBbits.LATB6=0;
                LATBbits.LATB4=1;
                state=3;
                row=2;
            }else{
                state=4;
            }
           break;
        case 3:
            if (PORTBbits.RB3 && PORTBbits.RB1 && PORTBbits.RB5){
                LATBbits.LATB2=1;
                LATBbits.LATB7=1;
                LATBbits.LATB6=1;
                LATBbits.LATB4=0;
                row=3;
                state=0;
            }else{
                state=4;
            }
           break;
        case 4:
            state=5;
            TI_ResetTics(debouncerTimer);
            break;
        case 5:
            if (TI_GetTics(debouncerTimer) >= TI_BOUNCES){
                if (PORTBbits.RB3 && PORTBbits.RB1 && PORTBbits.RB5){
                    state = 0;
                } else{
                    keyPressed = 1;
                    playMusic(getDigit());
                    state = 6;
                }
            }
            break;
        case 6:
            if (PORTBbits.RB3 && PORTBbits.RB1 && PORTBbits.RB5){
                TI_ResetTics(debouncerTimer);
                state = 7;
            }
            break;
        case 7:
            if (TI_GetTics(debouncerTimer) >= TI_BOUNCES){
                if (PORTBbits.RB3 && PORTBbits.RB1 && PORTBbits.RB5){
                    stopMusic();
                    keyPressed = 0;
                    state = 0;
                } else {    
                    state = 6;
                }
            }
            break;
    }
}


void smsLoop(void){
    switch(stateSMS){
        //LATAbits.LATA4 = pressed;
        case 0:
            //LATC = available;
            
            if(keyPressed){
                stateSMS=1;
                currentKey = getDigitValue();
                nextKey = 1;
                if(numPressed != 0 && (previousKey != currentKey || TI_GetTics(SMSTimer) >= TI_SECOND)){
                    nextKey = 0; // reset but new value;
                    previousKey = currentKey;
                    numPressed = 0;
                    
                }
            }else{
              previousKey=currentKey;
            }
            break;

        case 1:
            numPressed++;
            stateSMS = 2;
            break;

        case 2:
            if (!keyPressed){
                TI_ResetTics(SMSTimer);

                stateSMS = 0;
            }
            break;
    }
}


char isPressed(void) {
	//check columns return 1 if pressed
	return keyPressed && (stateSMS == 1);
}

int getColumn (void){
    if (!PORTBbits.RB3){ return 0;}
    if (!PORTBbits.RB1){ return 1;}
    return 2; 
}

char getDigit(void){
   if (!PORTBbits.RB1) {
        return row*3;
    } else if (!PORTBbits.RB3) {
        return (row*3)+1;
    } else {
        return (row*3)+2;
    }
}


char getCharacter(void){
    char digit = getDigitValue();
    if ((digit - '0' >= 2 && digit - '0' <= 9) || (digit - '0' == 0)){
        if(digit - '0' == 0){
            if(numPressed > 2){
                numPressed = 1;
            }
            if(numPressed == 1){
                return ' ';
            }else if(numPressed == 2){
                return '0';
            }
        }else if (SMSVALUES[digit - '0' - 1][(numPressed-1)] == '\0'){
            numPressed = 1;
        }
        return SMSVALUES[digit - '0' - 1][(numPressed-1)];
    }
    return digit;
}

char getDigitValue(void){
    return DIGITVALUES[(row*3)+getColumn()];
}


char isNextKey(void) {
    return nextKey;
}
