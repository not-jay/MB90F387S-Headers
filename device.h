#ifndef __device_h
#define __device_h

#include "device_com.h"
#include "fujitsu.h"

struct device {
    unsigned char   *dir;
    unsigned char   *bit;
    int             port;
};

typedef struct device Device;

Device          defineDevice(ByteMemAddress, ByteMemAddress, int);
void            setDirection(Device *, DataDirection);
unsigned char   getDirection(Device *);
void            setData(Device *, unsigned char);
unsigned char   getData(Device *);

Device defineDevice(ByteMemAddress dir, ByteMemAddress bit, int port) {
	Device d;
	
	d.dir = (unsigned char *)dir;
	d.bit = (unsigned char *)bit;
	d.port = port;
	
	return d;
}

void setDirection(Device *d, DataDirection dir) {
    if(d->port == BYTE)
        *d->dir = (unsigned char)dir;
    else
        *d->dir = (unsigned char)((*d->dir & ~(1 << d->port)) | (dir << d->port));
}

unsigned char getDirection(Device *d) {
    if(d->port == BYTE)
        return *d->dir;
    else
        return (unsigned char)((*d->dir >> d->port) & 0x01);
}

void setData(Device *d, unsigned char data) {
    if(d->port == BYTE)
        *d->bit = data;
    else
    	*d->bit = (unsigned char)((*d->bit & ~(1 << d->port)) | (data << d->port));
}

unsigned char getData(Device *d) {
    if(d->port == BYTE)
        return *d->bit;
    else
        return (unsigned char)((*d->bit >> d->port) & 0x01);
}

#endif
