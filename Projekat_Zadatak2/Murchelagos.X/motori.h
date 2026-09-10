/* 
 * File:   motori.h
 * Author: student
 *
 * Created on 27. mart 2026., 12.34
 */

#ifndef MOTOR_H
#define	MOTOR_H

/// library for MCU
#include <p30fxxxx.h>

// IN1, IN2, Prvi motor
#define IN1     LATBbits.LATB9 
#define IN2     LATBbits.LATB10
// IN3, IN4, Drugi motor
#define IN3     LATBbits.LATB12
#define IN4     LATBbits.LATB11

// Pinovi
void MotorPins();

// Stani
void Zaustavi();

// 
void IdiNaprijed();

// Rikverc
void Nazad();

// Skreni lijevo
void Lijevo();

// Skreni Desno
void Desno();


#endif	/* MOTOR_H */
