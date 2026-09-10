/* 
 * File:   pwm.h
 * Author: student
 *
 * Created on 27. mart 2026., 12.16
 */

#ifndef PWM_H
#define	PWM_H

///library for MCU
#include <p30fxxxx.h>

/// Function for initializing PWM module
void InitPWM();

/// Function for setting duty cycle for PWM1
void MotorPWM1(int duty_cycle);

/// Function for setting duty cycle for PWM2
void MotorPWM2(int duty_cycle);

#endif	/* PWM_H */

