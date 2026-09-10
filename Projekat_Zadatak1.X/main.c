#include<p30Fxxxx.h>
#include <stdlib.h>
#include <outcompare.h>

#include "timer1.h"
#include "timer2.h"
#include "glcd.h"
#include "slike.h"
#include "newfile.h"

_FOSC(CSW_ON_FSCM_OFF & XT_PLL4);//instruction takt je isti kao i kristal
                                 //_FOSC(CSW_FSCM_OFF & XT);//deli sa 4
_FWDT(WDT_OFF);
_FGS(CODE_PROT_OFF);

unsigned char m,n,x,y_senzor,i,p=0;
unsigned int match_value,pauza,pauza2,brojac,br1,br2,br3;//deo za frekvenciju prekida tajmera
unsigned int sirovi0,sirovi1,sirovi2,sirovi3;//ocitavanje adc konverzije    
unsigned int brojac2_ms,stoperica2,ms2,sekund2;//tajmer1
unsigned int X, Y,x_vrednost, y_vrednost;
unsigned int temp0,temp1; //za touch


#define DRIVE_A PORTCbits.RC13
#define DRIVE_B PORTCbits.RC14

#define xtal 25000000
#define BUZZER_LAT  LATAbits.LATA11 
#define BUZZER_TRIS TRISAbits.TRISA11


/*******************************************************************
//funkcija za kasnjenje u milisekundama
*********************************************************************/
void Delay_ms (int vreme2)//funkcija za kasnjenje u milisekundama
	{
		stoperica2 = 0;
		while(stoperica2 < vreme2);
	}

void __attribute__ ((__interrupt__)) _T2Interrupt(void) // svakih 1ms
{
	TMR2 =0;
    
    ms2=1;//fleg za milisekundu ili prekid;potrebno ga je samo resetovati u funkciji
    stoperica2++;//brojac za funkciju Delay_ms
    
	IFS0bits.T2IF = 0;    
}

void ConfigureTSPins(void)
{
	TRISCbits.TRISC13=0;
    TRISCbits.TRISC14=0;
}

void __attribute__((__interrupt__)) _T1Interrupt(void) {
  TMR1 =0;
  IFS0bits.T1IF = 0;   
}  

void initUART1(void)
{
    U1BRG=0x0040;//baud rate 9600
    U1MODEbits.ALTIO = 0;
    IEC0bits.U1RXIE = 1;
    U1STA&=0xfffc;
    U1MODEbits.UARTEN=1;
    U1STAbits.UTXEN=1;
}

void __attribute__((__interrupt__)) _U1RXInterrupt(void) 
{
    IFS0bits.U1RXIF = 0;
} 

void __attribute__((__interrupt__)) _ADCInterrupt(void) 
{							
	sirovi0=ADCBUF0;//x
	sirovi1=ADCBUF1;//y  
    sirovi2=ADCBUF2;//fotootpornik
    sirovi3=ADCBUF3;//mq3
    
    temp0=sirovi0;
	temp1=sirovi1;
										
    IFS0bits.ADIF = 0;
} 

/*********************************************************************
* Ime funkcije      : WriteUART1                            		 *
* Opis              : Funkcija upisuje podatke u registar U1TXREG,   *
*                     za slanje podataka    						 *
* Parameteri        : unsigned int data-podatak koji zelimo poslati  *
* Povratna vrednost : Nema                                           *
*********************************************************************/

void WriteUART1(unsigned int data)
{
	while (U1STAbits.TRMT==0);
    if(U1MODEbits.PDSEL == 3)
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}
/***********************************************************************
* Ime funkcije      : WriteUART1dec2string                     		   *
* Opis              : Funkcija salje 4-cifrene brojeve (cifru po cifru)*
* Parameteri        : unsigned int data-podatak koji zelimo poslati    *
* Povratna vrednost : Nema                                             *
************************************************************************/
void WriteUART1dec2string(unsigned int data)
{
	unsigned char temp;

	temp=data/1000;
	WriteUART1(temp+'0');
	data=data-temp*1000;
	temp=data/100;
	WriteUART1(temp+'0');
	data=data-temp*100;
	temp=data/10;
	WriteUART1(temp+'0');
	data=data-temp*10;
	WriteUART1(data+'0');
}

void Delay(unsigned int N)
{
	unsigned int i;
	for(i=0;i<N;i++);
}


void Touch_Panel (void)
{
// vode horizontalni tranzistori
	DRIVE_A = 1;  
	DRIVE_B = 0;
    
    LATCbits.LATC13=1;
    LATCbits.LATC14=0;
     
    Delay(500);
	// ocitavamo x	
	x_vrednost = temp0;//temp0 je vrednost koji nam daje AD konvertor na BOTTOM pinu		

	// vode vertikalni tranzistori
    LATCbits.LATC13=0;
    LATCbits.LATC14=1;
	DRIVE_A = 0;  
	DRIVE_B = 1;
    
    Delay(500);
	// ocitavamo y	
	y_vrednost = temp1;// temp1 je vrednost koji nam daje AD konvertor na LEFT pinu	
	
}


int main(int argc, char** argv)
{
  //------------------------------------------------------------------------------deo za init timera 1
    ConfigIntTimer1(T1_INT_PRIOR_1 & T1_INT_ON);
    WriteTimer1(0);
    match_value = (xtal/20000)-11;
    OpenTimer1(T1_ON & T1_GATE_OFF & T1_IDLE_CON &
        T1_PS_1_1 & T1_SYNC_EXT_OFF &
        T1_SOURCE_INT, match_value);
    

    ConfigureLCDPins();
    CORCONbits.PSV = 1;
    //deo mkoji erovatno nije potreban ali pisan je jer je oc pravio problem zbog pullupa
    IEC1bits.OC4IE = 0;// Disable the Interrupt bit in IEC Register 
    OC4CONbits.OCM = 0;//Turn off Output Compare 4 
    IFS1bits.OC4IF = 0;//Disable the Interrupt Flag bit in IFS Register 
    
   
     
    ConfigureAllPins ();
    for(pauza=32;pauza<6000;pauza++);
    GLCD_LcdInit();
    for(pauza=32;pauza<6000;pauza++);
    GLCD_ClrScr();

        TRISFbits.TRISF6=0;//konfigurisemo kao izlaz za servo motor
        TRISAbits.TRISA11=0;//izlaz za buzzer
		TRISBbits.TRISB11=1;//mq3
        TRISBbits.TRISB10=1;//fotootpornik
        TRISDbits.TRISD9=1;//pir senzor
            
        Init_T2();//inicijalizacija tajmera 2
		initUART1();//inicijalizacija UART-a
 		ADCinit();//inicijalizacija AD konvertora
        ConfigureADCPins();
        ConfigureTSPins();
		ADCON1bits.ADON=1;//pocetak Ad konverzije 
        
        TRISCbits.TRISC13=0;
        TRISCbits.TRISC14=0;
        
        GLCD_DisplayPicture (start);//start
        Touch_Panel();
        if(y_vrednost<2200 && y_vrednost>1800) p=1;//uslov za ulaz u while(1)
        
  while(p==1)
  {
   
      
    LcdSelectSide(RIGHT);
    LcdSelectStartline(0);
    LcdSelectSide(LEFT);
    LcdSelectStartline(0);
    GLCD_ClrScr();
	GoToXY(0,0);
    
    GLCD_DisplayPicture (grafik_bmp);
    for(x=15;x<=120;x++)
    {    
        if(x==17) GLCD_DisplayPicture (grafik_bmp);//ose grafika
        if(PORTDbits.RD9==1)
        {
            
            GLCD_DisplayPicture (grafik_bmp);
                         
        }
       else if(sirovi2<3500)//fotootpornik
        {
            GLCD_ClrScr();
            x=15;
            BUZZER_LAT = 1;//buzzer ukljucen
            Delay_ms(300);
            BUZZER_LAT = 0; //buzzer iskljucen
        }
        else if(sirovi3>3300)
        {
            LATFbits.LATF6=1;//servo motor
            Delay_ms (1);
            LATFbits.LATF6=0;     
        }
        else
        {   
            WriteUART1(' ');//pisanje na terminalu
            WriteUART1dec2string(sirovi2);//fotootpornik
            WriteUART1(' ');
            WriteUART1dec2string(sirovi3);//mq3
            WriteUART1(13);//enter
            WriteUART1('s');
            
            for(pauza=32;pauza<6000;pauza++);
            y_senzor=60-((sirovi3-1300)/50);//jacina y ose
            LcdSetDot(x,y_senzor);//crtanje grafika
            for(pauza=32;pauza<6000;pauza++);
        }
    }
  }//od whilea
  return 0;
}//od maina


