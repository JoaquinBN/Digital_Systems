#ifndef SIO_H
#define	SIO_H

#include <xc.h>
#include "pic18f4321.h"
#include "Timer.h"
   

void initSIO(void);

void setTX(unsigned char letter);

unsigned char checkTX(void);

unsigned char checkRX(void);

unsigned char getRX(void);
#endif	/* SIO_H */

