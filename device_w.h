#ifndef __device_w_h
#define __device_w_h

#include "device_com.h"
#include "fujitsu.h"

/* Similar to device.h but for word operations instead of byte */
/* Untested: Use at own risk */

struct device_w {
    unsigned char   *dir;
    unsigned int    *bit;
    int             port;
};

typedef struct device_w Device_W;

Device_W        defineDevice_w(WordMemAddress, WordMemAddress, int);
void            setDirection_w(Device_W *, DataDirection);
unsigned char   getDirection_w(Device_W *);
void            setData_w(Device_W *, unsigned int);
unsigned int    getData_w(Device_W *);

Device_W defineDevice_w(MemAddress dir, MemAddress bit, int port) {
	Device_W d;
	
	d.dir = (unsigned char *)dir;
	d.bit = (unsigned int *)bit;
	d.port = port;
	
	return d;
}

void setDirection_w(Device_W *d, DataDirection dir) {
    if(d->port == BYTE)
        *d->dir = (unsigned char)dir;
    else
        *d->dir = (unsigned char)((*d->dir & ~(1 << d->port)) | (dir << d->port));
}

unsigned char getDirection_w(Device_W *d) {
    if(d->port == BYTE)
        return *d->dir;
    else
        return (unsigned char)((*d->dir >> d->port) & 0x01);
}

void setData_w(Device_W *d, unsigned char data) {
    if(d->port == BYTE)
        *d->bit = data;
    else
    	*d->bit = (unsigned char)((*d->bit & ~(1 << d->port)) | (data << d->port));
}

unsigned int getData_w(Device_W *d) {
    if(d->port == BYTE)
        return *d->bit;
    else
        return (unsigned char)((*d->bit >> d->port) & 0x01);
}

#endif
