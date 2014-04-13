/**
 * Parking Lot Management System (PMS) v1
 *
 * Changelog:
 *  - Sensors set LEDs directly, instead of interfacing them via MB90F387S
 *  - Moved sevenseg from Port 5 to Port 2
 *  - Moved stepper motor from Port 2 to Port 3
 */

#include "_ffmc16.h"
#include "extern.h"
#include "monitor.h"
#include "..\boolean.h"
#include "..\device.h"
#include "..\steppermotor.h"
#include "..\sevenseg.h"

void main() {
    Device sensor[4];
    Device sevenseg = defineDevice(DDR2, PDR2, BYTE);
    Stepper stepper = defineStepper(DDR3, PDR3, 0, 3);
    int i, lastFree = 0, freeSlots = 0;

    for(i = 0; i < 4; i++) {
        sensor[i] = defineDevice(DDR1, PDR1, i);
        setDirection(&sensor[i], INPUT);
    }
    setResolution(&stepper, 0.9f);
    initSevenSeg(&sevenseg); //contains sleep(1000);

    __set_il(7);
    __EI();

    //Logic:
    //  Read sensors
    //  Display free slots (mu update lng kung ni change ang value)
    //  change stepper's target angle if conditions are true (optimized na ni, dli mu re-set sa angles if same na)
    //  single step sa stepper motor (optimized to not move at all kung na reach na ang target angle) :)

    //With optimization in mind, ang /always/ na mu fire na functions kay:
    // - sensor reading

    //so "overkill" na kung mag task scheduling
    //since, wala ma'y tasks na pde ma parallel ug buhat ani

    display(&sevenseg, 0, true);
    while(true) {
        //Read sensors
        //HIGH  - Wala'y na detect ang sensor
        //LOW   - Naa'y something sa sensor
        for(i = 0; i < 4; i++) {
            //if HIGH/true, therefore, free ang slot
            //then set LEDs accordingly (basically follow lng sa status sa sensor)
            if(getData(&sensor[i])) freeSlots++;
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
