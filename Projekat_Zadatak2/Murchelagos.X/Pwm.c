#include <p30fxxxx.h>

#include "pwm.h"
#include "tajmer1.h"


/*void InitPWM()
{
    TRISDbits.TRISD0 = 0;   // output pin ENA
    TRISDbits.TRISD1 = 0;   // output pin ENB
    
    //the PWM mode of of the output compare module without input fault protection
    OC1CONbits.OCM=0b110; 
    OC2CONbits.OCM=0b110; 
    // Timer 3 as the source for the output compare module
    OC1CONbits.OCTSEL = 1;   
    OC2CONbits.OCTSEL = 1; 
    //initial PWM duty cycle
    OC1R = 0;  //Output Compare 1 Register
    OC2R = 0;  //Output Compare 1 Register
    OC1RS = 250;   //Output Compare 1 Secondary Register
    OC2RS = 250;   //Output Compare 2 Secondary Register
    
    // turn on timer 3
    //T3CONbits.TON = 1; 
}*/

/* 
 * @brief - Function for setting duty cycle for PWM1
 * @param duty_cycle - The value from 0 to 100
 * @return None
 */
/*void MotorPWM1(int duty_cycle)
{
    if(duty_cycle>=0 && duty_cycle<=100)
    {
        // Calculate the duty cycle based on the given value from 0.0 to 1.0
        float pwm_value = (float)duty_cycle * (TMR3_period-1) /100;

        // Set the new duty cycle
        OC1RS = (int)pwm_value;   
    }
}*/

/* 
 * @brief - Function for setting duty cycle for PWM2
 * @param duty_cycle - The value from 0 to 100
 * @return None
 */
/*void MotorPWM2(int duty_cycle)
{
    if(duty_cycle>=0 && duty_cycle<=100)
    {
        // Calculate the duty cycle based on the given value from 0.0 to 1.0
        float pwm_value = (float)duty_cycle * (TMR3_period-1) /100;

        // Set the new duty cycle
        OC2RS = (int)pwm_value;
    }
}
*/