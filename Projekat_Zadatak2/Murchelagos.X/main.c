//Grupa 2
#include <stdio.h>
#include <stdlib.h>
#include <p30fxxxx.h>
#include <string.h>
#include <outcompare.h>
#include "motori.h"
#include "pwm.h"
#include "tajmer1.h" 

_FOSC(CSW_FSCM_OFF & XT_PLL4);
_FWDT(WDT_OFF);

unsigned int brojac_ms, stoperica, ms, sekund, r, sirovi0;

/********************************************************************
// Kasnjenje u milisekundama
*********************************************************************/
void Delay_ms(int vreme)
{
    stoperica = 0;
    while (stoperica < vreme);
}

/********************************************************************
// Kasnjenje u mikrosekundama
*********************************************************************/
void Delay_us(unsigned int us)
{
    unsigned int i;
    for (i = 0; i < us * 10; i++);
}

/********************************************************************
// Timer1 interrupt - svakih 1ms
*********************************************************************/
void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    TMR1 = 0;
    ms = 1;
    brojac_ms++;
    stoperica++;
    if (brojac_ms == 1000)
    {
        brojac_ms = 0;
        sekund = 1;
    }
    IFS0bits.T1IF = 0;
}

/********************************************************************
// ADC interrupt - Sharp senzor
*********************************************************************/
void __attribute__((__interrupt__)) _ADCInterrupt(void)
{
    sirovi0 = ADCBUF0;
    IFS0bits.ADIF = 0;
}

/********************************************************************
// UART2 init - Bluetooth
*********************************************************************/
void initUART2(void)
{
    U2BRG = 0x0040;         // 9600 baud za 10MHz
    U2STA &= 0xfffc;
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
}

/********************************************************************
// UART2 slanje stringa
*********************************************************************/
void UART2_Send_String(const char* str)
{
    while (*str)
    {
        while (U2STAbits.UTXBF);
        U2TXREG = *str++;
    }
}

/********************************************************************
// UART2 slanje integera
*********************************************************************/
void UART2_Send_Int(unsigned int broj)
{
    char buf[6];
    sprintf(buf, "%u", broj);
    UART2_Send_String(buf);
}

/********************************************************************
// HC-SR04 mjerenje udaljenosti u cm
// senzor=1 ? lijevi (TRIGGER=RF1, ECHO=RF0)
// senzor=2 ? desni  (TRIGGER=RC14, ECHO=RD9)
*********************************************************************/
unsigned int Izmjeri_cm(int senzor)
{
    unsigned int trajanje = 0;
    unsigned int timeout = 0;

    if (senzor == 1)
    {
        // Trigger puls
        LATFbits.LATF1 = 0;
        Delay_us(2);
        LATFbits.LATF1 = 1;
        Delay_us(10);
        LATFbits.LATF1 = 0;

        // Cekaj ECHO HIGH
        timeout = 0;
        while (PORTFbits.RF0 == 0)
        {
            timeout++;
            if (timeout > 30000) return 999;
        }

        // Mjeri trajanje ECHO HIGH
        trajanje = 0;
        while (PORTFbits.RF0 == 1)
        {
            trajanje++;
            Delay_us(1);
        }
    }
    else if (senzor == 2)
    {
        // Trigger puls
        LATCbits.LATC14 = 0;
        Delay_us(2);
        LATCbits.LATC14 = 1;
        Delay_us(10);
        LATCbits.LATC14 = 0;

        // Cekaj ECHO HIGH
        timeout = 0;
        while (PORTDbits.RD9 == 0)
        {
            timeout++;
            if (timeout > 30000) return 999;
        }

        // Mjeri trajanje ECHO HIGH
        trajanje = 0;
        while (PORTDbits.RD9 == 1)
        {
            trajanje++;
            Delay_us(1);
        }
    }

    return trajanje / 20 * 3;
}

/********************************************************************
// MAIN
*********************************************************************/
int main(int argc, char** argv)
{
    unsigned char c;
    unsigned int dist_lijevi, dist_desni;
    unsigned char auto_aktivan = 0;

    // UART2 pinovi
    TRISFbits.TRISF4 = 1;   // U2RX ulaz
    TRISFbits.TRISF5 = 0;   // U2TX izlaz

    // Sharp senzor
    TRISBbits.TRISB0 = 1;   // AN0 ulaz

    // Lijevi HC-SR04
    TRISFbits.TRISF1 = 0;   // TRIGGER izlaz
    TRISFbits.TRISF0 = 1;   // ECHO ulaz

    // Desni HC-SR04
    TRISCbits.TRISC14 = 0;  // TRIGGER izlaz
    TRISDbits.TRISD9  = 1;  // ECHO ulaz

    // Inicijalizacije
    MotorPins();
    Init_T1();
    initUART2();
    ADCinit();
    ADCON1bits.ADON = 1;

    // PWM konfiguracija 
    T2CON = 0;
    TMR2 = 0;
    PR2 = 249;                  // 5 kHz
    T2CONbits.TCKPS = 0b01;    // prescaler 1:8

    OC1CON = 0;
    OC1R  = 0;
    OC1RS = 145;                // faktor ispune
    OC1CONbits.OCM = 0b110;    // PWM mod

    T2CONbits.TON = 1;          // start PWM

 while (1)
{

    if (auto_aktivan == 1)
    {   
        
        Pravo();
        Naprijed();
        OC1RS = 150;
        Delay_ms(50);
        OC1RS = 120; 
        if (sirovi0 > 220 && sirovi0 < 500)
        {
            UART2_Send_String("Prepreka ispred!\r\n");
            
            Zaustavi();
            // Izmjeri bocne senzore
            dist_lijevi = Izmjeri_cm(1);
            UART2_Send_String("Lijevi: ");
            UART2_Send_Int(dist_lijevi);
            UART2_Send_String(" cm\r\n");
            
            dist_desni  = Izmjeri_cm(2);
            UART2_Send_String("Desni: ");
            UART2_Send_Int(dist_desni);
            UART2_Send_String(" cm\r\n");
            Delay_ms(500);  
            dist_lijevi = Izmjeri_cm(1);
            dist_desni  = Izmjeri_cm(2);
            
            if (dist_desni <= 30 && dist_lijevi > 25)
            {
                
                // Prepreka desno ? lijevo
                Naprijed();
                OC1RS = 180;
                Delay_ms(50);
                OC1RS = 130; 
                Lijevo();
                
                UART2_Send_String("Prepreka desno - idem lijevo!\r\n");
                Delay_ms(900);
                Zaustavi();
                Pravo();
                Delay_ms(150);
                dist_lijevi = Izmjeri_cm(1);
                dist_desni  = Izmjeri_cm(2);
                
            }
            else if (dist_lijevi <= 25 && dist_desni > 25)
            {
                
                // Prepreka lijevo ? desno
                Naprijed();
                OC1RS = 180;
                Delay_ms(50);
                OC1RS = 130; 
                Desno();
               
                UART2_Send_String("Prepreka lijevo - idem desno!\r\n");
                Delay_ms(900);   
                Zaustavi();
                Pravo();
                Delay_ms(150);
                dist_lijevi = Izmjeri_cm(1);
                dist_desni  = Izmjeri_cm(2);
            }
            else if (dist_lijevi > 25 && dist_desni > 25)
            {
                
                // Oba slobodna ? desno
                Naprijed();
                OC1RS = 180;
                Delay_ms(50);
                OC1RS = 130; 
                Desno();
                
                Delay_ms(900);
                UART2_Send_String("Idem desno!\r\n");
                Zaustavi();
                Pravo();
                Delay_ms(150);
                dist_lijevi = Izmjeri_cm(1);
                dist_desni  = Izmjeri_cm(2);
                
            }
        else if(dist_lijevi < 27 && dist_desni < 27)
        {
                //oba blokirana a prepreka detektovana ali je opet daleko
                Naprijed();
                OC1RS = 180;
                Delay_ms(50);
                OC1RS = 130; 
                Delay_ms(150);
                Zaustavi();
                dist_lijevi = Izmjeri_cm(1);
                dist_desni  = Izmjeri_cm(2);
        }
        }
        if(sirovi0 >=500 && sirovi0<2500)
        {
                Zaustavi();
                Delay_ms(200);
                Pravo();
                Delay_ms(200);
                Nazad();
                OC1RS = 180;
                Delay_ms(50);
                OC1RS = 145; 
                Delay_ms(450);
                UART2_Send_String("Idem nazad!\r\n");
                Naprijed();
        }
    }
    if (U2STAbits.URXDA)
    {
        c = U2RXREG;

        if (c == 'n')
        {
            UART2_Send_String("Idem naprijed!\r\n");
            LATDbits.LATD1 = 1;
            auto_aktivan = 1;
            Naprijed();
        }
        if (c == 'z')
        {
            UART2_Send_String("Zaustavljam!\r\n");
            LATDbits.LATD1 = 0;
            auto_aktivan = 0;
            Zaustavi();
        }
    }

    // Ispis svake sekunde
    if (sekund == 1)
    {
        sekund = 0;
        
        UART2_Send_String("ADC Sharp: ");
        UART2_Send_Int(sirovi0);
        UART2_Send_String("\r\n");

        if (dist_lijevi >= 999)
            UART2_Send_String("Lijevi: nema objekta\r\n");
        else
        {
            UART2_Send_String("Lijevi: ");
            UART2_Send_Int(dist_lijevi);
            UART2_Send_String(" cm\r\n");
        }
        if (dist_desni >= 999)
            UART2_Send_String("Desni: nema objekta\r\n");
        else
        {
            UART2_Send_String("Desni: ");
            UART2_Send_Int(dist_desni);
            UART2_Send_String(" cm\r\n");
        }
    }
}
    return (EXIT_SUCCESS);
}