/*
 * File:   SERVO.c
 * Author: root
 *
 * Created on January 13, 2021, 9:16 PM
 */


#include "xc.h"
#include <p30F4013.h>
#include <stdio.h>
#define MIPS 30
#define B9 LATBbits.LATB9
#define B10 LATBbits.LATB10
#define B11 LATBbits.LATB11
#define B12 LATBbits.LATB12

_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void ADC(void);
void pwm_conf(void);

uint16_t AD=0;
uint16_t AD1=0;
float ADCf = 0.0;
int ADCint = 0;

int main(void) 
{
    ADC();
    pwm_conf();
    uint16_t AD=0;
    uint16_t AD1=0;  
    IEC0bits.ADIE=1;
    IFS0bits.ADIF=0;
    while (1) // repeat continuously

    {
        ADCON1bits.SAMP = 1;
        delay_us(100); 	// 
        ADCON1bits.SAMP = 0; 		// start Converting
        while (IFS0bits.ADIF==1); 	// Espero que la interrupcion se habilite
        OC1RS=ADCint;
    }
    return 0;
}

void ADC()
{
    ADPCFG = 0b1111111111100111; 
    ADCON1 = 0x0000; 	// Register 18-1:
    ADCON2 = 0b0000010000000100;
    ADCHS = 0b0000000000000011;
    ADCSSL = 0b0000000000011000;		// Register 18-6: ADCSSL: A/D Input Scan Select Register
    ADCON3 = 0x0002;          
    
    ADCON1bits.ADON = 1;
}

void pwm_conf(void)
{
    TRISDbits.TRISD0=0;
    PR2=2342; //PERIODO DEL PWM (Tpwm/(TCY*PRESCALER))-1 -> Tpwm= [(PR2) + 1]*TCY*Prescaler
    TMR2=0; // DONDE ARRANCA EL TIMER 2
    OC1R=0; //TON PARA EL PRIMER CICLO
    OC1RS=0;
    OC1CONbits.OCM=0b110;
    OC1CONbits.OCTSEL=0;
    T2CONbits.T32=0; //SI QUEREMOS UN TIMER DE 32 O 16 BITS
    T2CONbits.TCKPS=0b11; //ELEGIMOS EL PRESCALER
    T2CONbits.TCS=0; //ELEGIMOS QUE QUEREMOS SEGUIR LAS INSTRUCCIONES INTERNAS COMO ORIGEN DEL TIMER
    T2CONbits.TON=1;//ENCENDEMOS EL TIMER2
}

void __attribute__ ((__interrupt__)) _ADCInterrupt(void)
{
    ADCON1bits.ADON = 0;   // this must be executed before the end of 17th conversion
    IFS0bits.ADIF = 0;
    AD = ADCBUF0;
    AD1 = ADCBUF1;
    ADCf = ((float)AD1*234.0)/4095.0;
    ADCint = (int)ADCf;
    ADCON1bits.ADON = 1;
}

void delay_ms (unsigned long delay_count)

	{
	delay_count=delay_count*MIPS*100;
	while(delay_count--);
	}


void delay_us (unsigned int delay_count)

	{
	delay_count=delay_count*MIPS/4;
	
	while(delay_count--);
	}