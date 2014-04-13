#ifndef __steppermotor_h
#define __steppermotor_h

#include "boolean.h"
#include "fujitsu.h"

typedef enum {CLOCKWISE, COUNTERCLOCKWISE} Rotation;

struct stepper {
    Device          *device[4];
    float           resolution;
    unsigned char   value;
    float           angle;
    float           targetAngle;
    Rotation        rotation;
};

typedef struct stepper Stepper;

Stepper     defineStepper(ByteMemAddress, ByteMemAddress, int);
void        setResolution(Stepper *, float);
void        setTargetAngle(Stepper *, float);
void        setRotation(Stepper *, Rotation);
void        leftShift(Stepper *);
void        rightShift(Stepper *);
void        step(Stepper *);

Stepper defineStepper(ByteMemAddress dir, ByteMemAddress bit, int lsb) {
    int i;
    Device bits[4];
    Stepper stepper;

    for(i = 0; i < 4; i++) {
        bits[i] = defineDevice(dir, bit, lsb+i);
        setDirection(&bits[i], OUTPUT);

        stepper.device[i] = &bits[i];
    }

    stepper.resolution  = 0;
    stepper.value       = 0x10;
    stepper.angle       = 0;
    stepper.targetAngle = 0;
    stepper.rotation    = CLOCKWISE;

    return stepper;
}

void setResolution(Stepper *s, float resolution) {
    s->resolution = resolution;
}

void setTargetAngle(Stepper *s, float targetAngle) {
    s->targetAngle = targetAngle;
}

void setRotation(Stepper *s, Rotation rotation) {
    s->rotation = rotation;
}

void leftShift(Stepper *s) {
    setData(s->device[0], getData(s->device[1]));
    setData(s->device[1], getData(s->device[2]));
    setData(s->device[2], getData(s->device[3]));
    setData(s->device[3], getData(s->device[0]));
}

void rightShift(Stepper *s) {
    setData(s->device[0], getData(s->device[3]));
    setData(s->device[1], getData(s->device[0]));
    setData(s->device[2], getData(s->device[1]));
    setData(s->device[3], getData(s->device[2]));
}

void step(Stepper *s) {
    if(s->resolution != 0 && s->angle != s->targetAngle) {
        switch(s->rotation) {
        default:
        case CLOCKWISE:
            rightShift(s);
            s->angle -= s->resolution;
            break;
        case COUNTERCLOCKWISE:
            leftShift(s);
            s->angle += s->resolution;
            break;
        }
    }
}

#endif
