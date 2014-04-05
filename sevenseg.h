#ifndef __sevenseg_h
#define __sevenseg_h

#include "boolean.h"
#include "device.h"
#include "sleep.h"

#define DOT_ON  0x80
#define SEG_OFF 0x00
#define SEG_ON  ~(SEG_OFF)

void            initSevenSeg(Device *d);
void            display(Device *, int, boolean);
unsigned char   decodeInt(int);

void initSevenSeg(Device *d) {
	setDirection(d, 0xff);

    setData(d, SEG_ON);
    sleep(1000);
    setData(d, SEG_OFF);
}

void display(Device *d, int num, boolean dotOn) {
    unsigned char number = decodeInt(num);
    if(dotOn) number |= DOT_ON;
    setData(d, number);
}

unsigned char decodeInt(int number) {
    switch(number) {
    case 0:		return 0x3f;
    case 1:		return 0x06;
    case 2:		return 0x5b;
    case 3:		return 0x4f;
    case 4:		return 0x66;
    case 5:		return 0x6d;
    case 6:		return 0x7d;
    case 7:		return 0x07;
    case 8:		return 0x7f;
    case 9:		return 0x6f;
    case 10:	return 0x77;
    case 11:	return 0x7c;
    case 12:	return 0x39;
    case 13:	return 0x5e;
    case 14:	return 0x79;
    case 15:	return 0x71;
    default:	return SEG_OFF;
    }
}

#endif
