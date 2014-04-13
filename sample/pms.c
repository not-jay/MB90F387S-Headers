/**
 * Parking Lot Management System (PMS) v0
 */

#include "_ffmc16.h"
#include "extern.h"
#include "monitor.h"
#include "..\boolean.h"
#include "..\device.h"
#include "..\steppermotor.h"
#include "..\sevenseg.h"

void main() {
    Device leds[4];
    Device sensor[4];
    Device sevenseg = defineDevice(DDR5, PDR5, BYTE);
    Device ader = defineDevice(ADER, ADER, BYTE);
    Stepper stepper = defineStepper(DDR2, PDR2, 4);
    int i, lastFree = 0, freeSlots = 0;

    for(i = 0; i < 4; i++) {
        leds[i] = defineDevice(DDR2, PDR2, i);
        sensor[i] = defineDevice(DDR1, PDR1, i);

        setDirection(&leds[i], OUTPUT);
        setDirection(&sensor[i], INPUT);
    }
    setResolution(&stepper, 0.9f);
    setDirection(&ader, 0x00); //set port 5 to GPIO
    initSevenSeg(&sevenseg); //contains sleep(1000);

    __set_il(7);
    __EI();

    //Logic:
    //  Read sensors
    //  Set LEDs while counting free slots
    //  Display free slots (mu update lng kung ni change ang value)
    //  change stepper's target angle if conditions are true (optimized na ni, dli mu re-set sa angles if same na)
    //  single step sa stepper motor (optimized to not move at all kung na reach na ang target angle) :)

    //With optimization in mind, ang /always/ na mu fire na functions kay:
    // - sensor reading
    // - LED setting

    //so "overkill" na kung mag task scheduling
    //since, wala ma'y tasks na pde ma parallel ug buhat ani
    while(true) {
        //Read sensors
        //HIGH  - Wala'y na detect ang sensor
        //LOW   - Naa'y something sa sensor
        for(i = 0; i < 4; i++) {
            unsigned char isFree = getData(&sensor[i]);
            //if HIGH/true, therefore, free ang slot
            //then set LEDs accordingly (basically follow lng sa status sa sensor)
            if(isFree) freeSlots++;
            setData(&leds[i], isFree);
        }

        //Display freeSlots on 7seg (only update, if it needs updating)
        if(lastFree != freeSlots) display(&sevenseg, freeSlots, false);
        lastFree = freeSlots;
        freeSlots = 0;

        //if 0 ang freeSlots, set stepper to 45, counter
        //otherwise, set stepper to 0, clockwise
        //ang != 0 check dri is for mu skip siya sa 'sets' and save a few picoseconds :D
        if(freeSlots > 0 && stepper.targetAngle != 0) {
            setTargetAngle(&stepper, 0);
            setRotation(&stepper, CLOCKWISE);
        } else if(freeSlots <= 0 && stepper.targetAngle != 45) {
            setTargetAngle(&stepper, 45);
            setRotation(&stepper, COUNTERCLOCKWISE);
        }

        step(&stepper);
    }
}

/********************************************
    interrupt routine
/********************************************/
/* Vector Table */
#pragma section INTVECT,locate=0xfffc00
#pragma intvect _start          0x8     0x0 /* Reset Vector */
