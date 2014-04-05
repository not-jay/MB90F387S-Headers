#ifndef __lcd_h
#define __lcd_h

#include "device.h"
#include "sleep.h"

struct lcd {
    Device  *rs;
    Device  *rw;
    Device  *e;
    Device  *bf;
    Device  *data;
};

typedef struct lcd LCD;

typedef enum {DECREMENT, INCREMENT} CurPos;
typedef enum {NO_DISPLAY_SHIFT, DISPLAY_SHIFT} DisplayShift;
typedef enum {DISPLAY_OFF, DISPLAY_ON} Display;
typedef enum {CURSOR_OFF, CURSOR_ON} Cursor;
typedef enum {BLINK_OFF, BLINK_ON} CursorBlink;
typedef enum {MOVE_CURSOR, SHIFT_DISPLAY} SC;
typedef enum {SHIFT_LEFT, SHIFT_RIGHT} RL;
typedef enum {I4BIT, I8BIT} BitInterface;
typedef enum {SINGLE_LINE, DUAL_LINE} DutyLine;
typedef enum {DM5X7, DM5X10} MatrixSize;

LCD             defineLCD(Device *, Device *, Device *, Device *, Device *);
void            initLCD(LCD *);
void            send4bitInst(LCD *, unsigned char);
void            send8bitInst(LCD *, unsigned char);
void            readBusyFlag(LCD *);
void            sendData(LCD *, unsigned char);
void            moveCursor(LCD *, int, int);
#define         clearDisplay()  0x01
#define         cursorHome()    0x02
unsigned char   entryMode(CurPos, DisplayShift);
unsigned char   display(Display, Cursor, CursorBlink);
unsigned char   cursorDisplayShift(SC, RL);
unsigned char   functionSet(BitInterface, DutyLine, MatrixSize);
unsigned char   setDDRAMAddress(unsigned char);

LCD defineLCD(Device *rs, Device *rw, Device *e,
              Device *bf, Device *data) {
    LCD lcd;

    lcd.rs     = rs;
    lcd.rw     = rw;
    lcd.e      = e;
    lcd.bf     = bf;
    lcd.data   = data;

    setDirection(lcd.rs  , OUTPUT);
    setDirection(lcd.rw  , OUTPUT);
    setDirection(lcd.e   , OUTPUT);
    setDirection(lcd.bf  , INPUT);
    setDirection(lcd.data, OUTPUT);

    return lcd;
}

void initLCD(LCD *lcd) {
	sleep(150);			//LCD startup
	send4bitInst(lcd, 0x03);
	sleep(50);			//4.1 ms delay
	send4bitInst(lcd, 0x03);
	usleep(1320);		//100 us delay
	send4bitInst(lcd, 0x03);
	sleep(50);			//4.1 ms delay
	send4bitInst(lcd, 0x02);
	sleep(600);         //wait for instruction to finish
    send8bitInst(lcd, functionSet(I4BIT, DUAL_LINE, DM5X7));
    send8bitInst(lcd, display(DISPLAY_OFF, CURSOR_OFF, BLINK_OFF));
    send8bitInst(lcd, clearDisplay());
    send8bitInst(lcd, entryMode(INCREMENT, DISPLAY_SHIFT));
    send8bitInst(lcd, display(DISPLAY_ON, CURSOR_ON, BLINK_OFF));
}

void send4bitInst(LCD *lcd, unsigned char inst) {
    setData(lcd->data, inst);
    setData(lcd->rs, 0);
    setData(lcd->rw, 0);
    setData(lcd->e , 1);
    usleep(330);
    setData(lcd->e , 0);
    readBusyFlag(lcd);
}

void send8bitInst(LCD *lcd, unsigned char inst) {
    setData(lcd->data, inst >> 4);
    setData(lcd->rs  , 0);
    setData(lcd->rw  , 0);
    setData(lcd->e   , 1);
    usleep(330);
    setData(lcd->e   , 0);
    usleep(330);
    setData(lcd->data, inst);
    setData(lcd->rs  , 0);
    setData(lcd->e   , 1);
    usleep(330);
    setData(lcd->e   , 0);
    readBusyFlag(lcd);
}

void readBusyFlag(LCD *lcd) {
    unsigned char flag;

    setData(lcd->data, 0x00);
    setData(lcd->rs  , 0);
    setData(lcd->rw  , 1);
    setData(lcd->e   , 1);

    do
        flag = getData(lcd->bf);
    while(flag);

    setData(lcd->e   , 0);
    setData(lcd->data, 0xff);
}

void sendData(LCD *lcd, unsigned char data) {
    setData(lcd->data, data >> 4);
    setData(lcd->rs  , 1);
    setData(lcd->rw  , 0);
    setData(lcd->e   , 1);
    usleep(330);
    setData(lcd->e   , 0);
    usleep(330);
    setData(lcd->data, data);
    setData(lcd->rs  , 1);
    setData(lcd->e   , 1);
    usleep(330);
    setData(lcd->e   , 0);
    readBusyFlag(lcd);
}

void moveCursor(LCD *lcd, int line, int col) {
    int hex;

    switch(line) {
    default:
    case 1: hex = 0x80; break;
    case 2: hex = 0xC0; break;
    case 3: hex = 0x94; break;
    case 4: hex = 0xD4; break;
    }

    send8bitInst(lcd, hex + (col-1));
}

unsigned char entryMode(CurPos curpos, DisplayShift ds) {
	return 0x04|(curpos << 1)|ds;
}

unsigned char display(Display d, Cursor c, CursorBlink b) {
	return 0x08|(d << 2)|(c << 1)|b;
}

unsigned char cursorDisplayShift(SC sc, RL rl) {
	return 0x10|(sc << 3)|(rl << 2);
}

unsigned char functionSet(BitInterface bi, DutyLine dl, MatrixSize ms) {
	return 0x20|(bi << 4)|(dl << 3)|(ms << 2);
}

unsigned char setDDRAMAddress(unsigned char address) {
	return 0x80|address;
}

#endif
