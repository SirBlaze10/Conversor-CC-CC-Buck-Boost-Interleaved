#include "Epll.h"

#include "math.h"

#include "adc.h"
//_____________________________________________ EPLL________________________________
E_PLL_t epll = {
    0,
    0,

    W_FREQ,
    0,
    PI/2,

     Kapinha,
    0,

    {Kp, Ki, H_PE, 0, 0},
    {0, 0, H_PE},
    {0, 0, H_PE}
};



//___________________________________   função EPLL___________________________________
void EPLL_calc(void)
{
    float e_x_sinT = 0;
    float posPI = 0;
    float preIntg1 = 0;
    float A = 0;

    int aux = 0;


   // epll.voltageIN = (float)((int)(sensorSampleV-2047)/(1241))*54.54;
    aux = (int)ADC1_chn0_value;
    epll.voltageIN = (float)(aux-1850);
    epll.voltageIN = epll.voltageIN/1241;
    epll.voltageIN = epll.voltageIN*54.54;



    epll.error = epll.voltageIN - epll.voltageOUT;


    e_x_sinT = epll.error * sin(epll.Teta);


    epll.contPI.error_sum += epll.contPI.prev_error;
    if(epll.contPI.error_sum > INTG_SAT)
    {
        epll.contPI.error_sum = INTG_SAT;
    }
    if(epll.contPI.error_sum < -INTG_SAT)
    {
        epll.contPI.error_sum = -INTG_SAT;
    }
    posPI = e_x_sinT * epll.contPI.kP + (epll.contPI.kI * epll.contPI.samp_period)*epll.contPI.error_sum;
    epll.contPI.prev_error = e_x_sinT;



    epll.integral2.sum = epll.integral2.sum +  epll.integral2.prev;
    if(epll.integral2.sum > INTG_SAT)
    {
    epll.integral2.sum = INTG_SAT;
    }
    if(epll.integral2.sum < -INTG_SAT)
    {
        epll.integral2.sum = -INTG_SAT;
    }
    epll.Teta = epll.integral2.sum * epll.integral2.h;
    epll.integral2.prev = posPI + epll.wantedFreq;


    preIntg1 = cos(epll.Teta) * epll.error;


    epll.integral1.sum = epll.integral1.sum +  epll.integral1.prev;
    if(epll.integral1.sum > INTG_SAT)
    {
    epll.integral1.sum = INTG_SAT;
    }
    if(epll.integral1.sum < -INTG_SAT)
    {
        epll.integral1.sum = -INTG_SAT;
    }
    A = epll.K *  epll.integral1.sum * epll.integral1.h;
    epll.integral1.prev = preIntg1;


    epll.voltageOUT = A * cos(epll.Teta);
    
}


