#include <pic18f4321.h>
#include <xc.h>
#include "Joystick.h"
#include "Timer.h"
#include "Keypad.h"
#include "Menu.h"
#include "Audio.h"
#include "LcTLCD.h"
#include "SIO.h"
#include "EEPROM.h"

#pragma config OSC = HSPLL //INTIO2
#pragma config PBADEN = DIG
#pragma config MCLRE = OFF
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF

void HighInterruptRSI(void);
void main(void);
void __interrupt() RSI_High(void){
    //void interrupt() RSI_High(void){
    RSI_Timer0();
}

void main(void){
    TRISAbits.RA5 = 1;
    TI_Init();
    initJoystick();
    initKeypad();
    LcInit(2,16);
    initAudio();
    initMenu();
    initSysTimer();
    initSIO();
    while(1){
        audioLoop(); 
        joystickLoop();
        keypadLoop();
        smsLoop();
        sysTimerLoop();
        memoryLoop();
        menuLoop();
    }
}