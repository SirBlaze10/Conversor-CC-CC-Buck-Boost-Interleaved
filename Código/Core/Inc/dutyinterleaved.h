#ifndef _dutyinterleaved_H
#define _dutyinterleaved_H

#include "float.h"

extern float duty;
extern float sensorI_1;
extern float sensorI_2;

extern void duty_cycle_boost_calc(void);
extern void duty_cycle_buck_calc(void);

#endif
