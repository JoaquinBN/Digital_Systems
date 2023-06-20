#ifndef AUDIO_H
#define    AUDIO_H

#include <xc.h>
#include "Timer.h" 

void initAudio(void);

void audioLoop(void);

void playMusic(char aux);

void stopMusic(void);

#endif