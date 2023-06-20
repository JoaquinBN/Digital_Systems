#include <xc.h>
#include "Joystick.h"


static char state;
static char direction;
static char hasMoved;
static char x,y;

void initJoystick(void){
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    ADCON0 = 0x03;
    ADCON1 = 0x0C;
    ADCON2 = 0x44;

    hasMoved = 0;
    ADCON0bits.GO_DONE=1;
    ADCON0bits.CHS0=1;
    ADCON0bits.CHS1=0;
    state = 0;
}


void joystickLoop(void){
    switch (state){
        case 0:
            if (ADCON0bits.GO_DONE == 0){
                y = ADRESH;
                ADCON0bits.CHS0=1;
                ADCON0bits.CHS1=0;
                ADCON0bits.GO_DONE=1;
                state = 1;
            }
            break;
        case 1:
            if (ADCON0bits.GO_DONE == 0){
                x = ADRESH;
                ADCON0bits.CHS0=0;
                ADCON0bits.CHS1=1;
                state = 2;
            }
            break;
        case 2:
            state = 3;
            if (hasMoved) {
                if (y >= 100 && y <= 150) {
                    hasMoved = 0;
                }
            } else {
                if (x <= 30 || x >= 220 || y <= 30 || y >= 220) {
                    direction = (x <= 30 || y <= 30) ? 0 : 1;
                    hasMoved = 1;
                }
            }
            break;
        case 3:
            state = 0;
            ADCON0bits.GO_DONE=1;
            break; 
        case 4:
            break;
        default:
            state = 0;
            break;
    }

}


char getJoystickDown(void){
    return hasMoved && direction == 1;
}

char getJoystickUp(void){
    return hasMoved && direction == 0;
}

void joystickIdle(void){
    ADCON0bits.CHS0 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS2 = 1;
    state = 4;
}

void joystickAlive(void){
    ADCON0bits.CHS0 = 1;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS2 = 0;
    state = 0;
}