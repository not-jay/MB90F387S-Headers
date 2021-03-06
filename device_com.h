#ifndef __device_com_h
#define __device_com_h

/* Shared enums with device.h and device_w.h */

typedef enum {LOW, HIGH} InputLevel;
typedef enum {INPUT, OUTPUT} DataDirection;
typedef enum {ADER_GPIO, ADER_ANALOG = 0xff} Port5;
typedef enum {WORD = -2, BYTE = -1} PortType;

#endif
