#include <xc.h>
#include "Audio.h"


static char pTime;
static char state = 0;
static char period;
static char songTime;

void initAudio(void) {
    TRISCbits.TRISC2 = 0;
    TI_NewTimer(&pTime);
    TI_NewTimer(&songTime);
}

void audioLoop(void){
    switch(state){
        case 0:
            //song idle off
            LATCbits.LATC2=0;
            break;
        case 1:
            if (TI_GetTics(pTime)>= (period+1)){
                TI_ResetTics(pTime);
                LATCbits.LATC2=1;
                state = 2;
            }
            break;
        case 2:
            if (TI_GetTics(pTime)>= (period+1)){
                TI_ResetTics(pTime);
                LATCbits.LATC2=0;
                state = 1;
            }
            break;

    }
}


void stopMusic(void){
    state = 0;
} 

void playMusic(char aux){
    TI_ResetTics(pTime);
    period = aux; // set initial period
    state = 1;
}
