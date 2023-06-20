#include "SIO.h"

void initSIO(void){
    SPBRGH = 0;
    SPBRG = 172;
    BAUDCONbits.BRG16 = 1;
    BAUDCONbits.ABDEN = 0;
    TXSTAbits.BRGH = 1;
    TXSTAbits.SYNC = 0;  
    TXSTAbits.TXEN = 1;
    RCSTAbits.SPEN = 1;
    RCSTAbits.CREN = 1;
    
    TRISCbits.RC6 = 1;
    TRISCbits.RC7 = 1;
}

void setTX(unsigned char letter){
    TXREG = letter;
}

unsigned char checkTX(void){
    return TXSTAbits.TRMT;
}

unsigned char checkRX(void){
    return PIR1bits.RCIF;
}

unsigned char getRX(void){
    return RCREG;
}