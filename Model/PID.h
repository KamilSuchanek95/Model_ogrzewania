/*
 * PID.h
 *
 *  Created on: 10 lip 2018
 *      Author: kamil
 */

#ifndef PID_H_
#define PID_H_

typedef struct {
	int setpoint;
	int t[2];
	int U;
	int dt;
	int K_p;
	int K_i;
	int K_d;
	int out;
	int integral;
}PID_st_t;


extern int PID(int point);

#endif /* PID_H_ */
