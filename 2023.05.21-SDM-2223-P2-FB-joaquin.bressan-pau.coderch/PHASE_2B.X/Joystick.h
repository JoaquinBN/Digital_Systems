#ifndef JOYSTICK_H
#define    JOYSTICK_H

#include <xc.h> 

void joystickLoop(void);
void initJoystick(void);

char getJoystickUp(void);
char getJoystickDown(void);

void joystickIdle(void);
void joystickAlive(void);

#endif