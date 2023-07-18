#ifndef _E_PLL_H
#define _E_PLL_H

#include "float.h"
//________________ DEFINES _____________________________________
#define PI 3.141592

#define H_PE 0.00005       //periodo de amostragem      //  1/20000
#define W_FREQ 2*PI*50

#define Kp 1.5
#define Ki 0.01
#define Kapinha 40

#define INTG_SAT DBL_MAX




#define TIM_RATIO 28.76
#define TIM_RATIO2 0.527344




//____________________________________ estruturas _________________________________________
typedef struct Integral_s
{
    double sum;
    float prev;
    float h;
}Integral_t;

typedef struct PI_controler_s
{
    float kP;
    float kI;
    float samp_period;
    double error_sum;
    float prev_error;
}PI_controler_t;


typedef struct E_PLL_s
{
    float voltageIN;
    float voltageOUT;

    float wantedFreq;
    float error;
    float Teta;

    float K;
    float sum;

    PI_controler_t contPI;
    Integral_t integral1;
    Integral_t integral2;
}E_PLL_t;

//______________________ VARIAVEIS GLOBAIS ______________________________________

extern E_PLL_t epll;


//______________________ PROTOTYPES ______________________________________


extern void EPLL_calc(void);





#endif // _E_PLL_H
