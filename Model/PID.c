#include <stdio.h>
#include <stdlib.h>
#include "PID.h"

PID_st_t PID_st;
PID_st_t* PP_PID=&PID_st;

int PID(int point)
{

int pre_error =0;
int i;
for(i = 0; i<sizeof(PP_PID->t); i++)
{
PP_PID->integral += (PP_PID->setpoint - point) * PP_PID->dt;

PP_PID->U =  PP_PID->K_p * PP_PID->setpoint - point
				+ 
				PP_PID->K_i * PP_PID->integral
				+ 
				(((PP_PID->setpoint - point) - pre_error) * PP_PID->K_d) / PP_PID->dt;

pre_error = PP_PID->setpoint - point;
}

return PP_PID->U;
}
