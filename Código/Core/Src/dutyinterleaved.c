#include "math.h"

#include "adc.h"

#include "dutyinterleaved.h"

//________________ DEFINES _____________________________________

#define sensor1_sensitivity 0.0419
#define sensor2_sensitivity 0.0419

//______________________ VARIAVEIS GLOBAIS ______________________________________

float ki = 0.0, kp = 0.0;
float integral = 0.0, erro = 0.0;
float duty = 0.0;
float sensorI_1 = 0.0, rawVoltage_1 = 0.0;
float sensorI_2 = 0.0, rawVoltage_2 = 0.0;


void duty_cycle_buck_calc(void)
{
	kp=55;
	ki=0.05;
	
	int aux_1 = 0;	
	int aux_2 = 0;	
	
	aux_1 = (int)ADC1_chn4_value;
	rawVoltage_1 = (float) aux_1 * 3.3 / 4095;
	sensorI_1 = (rawVoltage_1-0.02) / sensor1_sensitivity;
	
	aux_2 = (int)ADC2_chn5_value;
	rawVoltage_2 = (float) aux_2 * 3.3 / 4095;
	sensorI_2 = (rawVoltage_2-0.02) / sensor2_sensitivity;
	
	erro = 16 - (sensorI_1+sensorI_2);
	integral += erro;
	duty = kp*erro + ki*integral;
}
	


void duty_cycle_boost_calc(void)
{
	ki = 0.1;
	kp = 10.0;
	
	int aux_1 = 0;	
	int aux_2 = 0;	
	
	aux_1 = (int)ADC1_chn4_value;
	rawVoltage_1 = (float) aux_1 * 3.3 * 2 / 4095;
	sensorI_1 = (rawVoltage_1 - 2.5) / sensor1_sensitivity;
	
	aux_2 = (int)ADC2_chn5_value;
	rawVoltage_2 = (float) aux_2 * 3.3 * 2 / 4095;
	sensorI_2 = (rawVoltage_2 - 2.5) / sensor2_sensitivity;
	
	erro = -16.32 - (sensorI_1+sensorI_2);
	integral += erro;
	duty = -(kp*erro + ki*integral);
}
