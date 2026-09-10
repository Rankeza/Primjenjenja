#include "motori.h"


void MotorPins()
{
    TRISBbits.TRISB9 = 0; // izlaz pin IN1
    TRISBbits.TRISB10 = 0; // izlaz pin IN2
    TRISBbits.TRISB11 = 0; // izlaz pin IN4
    TRISBbits.TRISB12 = 0; // izlaz pin IN3
    TRISDbits.TRISD0 = 0;//ENA izlaz
    TRISDbits.TRISD1 = 0;//ENB izlaz
    
    ADPCFGbits.PCFG9 = 1;//IN1
    ADPCFGbits.PCFG10 = 1;//IN2
    ADPCFGbits.PCFG11 = 1; //IN4
    ADPCFGbits.PCFG12 = 1; //IN3
}


void Zaustavi()
{
    IN1=0;
    IN2=0;
    IN3=0;
    IN4=0;
}
void Pravo()
{
    IN3=0;
    IN4=0;
}

void Naprijed()
{
    IN1=1;
    IN2=0; 
}

void Nazad()
{
    IN1=0;
    IN2=1;

}

void Desno()
{
    IN3=1;
    IN4=0;
}

void Lijevo()
{
    IN3=0;
    IN4=1; 
}


