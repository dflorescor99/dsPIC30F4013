/*
 * File:   pwm.c
 * Author: root
 *
 * Created on May 27, 2020, 6:07 PM
 */


void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void pwm_conf(void);

#include "xc.h"
#include <p30F4013.h>
#define MIPS 30


_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

int main(void) 
{
    pwm_conf();
    OC1RS=100;//7197*2; // TON DEL PWM
    OC2RS=10000;
    OC3RS=5000;
    OC4RS=20000;
    while (1);
}

void pwm_conf(void)
{
    //DEFINIMOS LOS PUERTOS DEL PWM COMO SALIDA DIGITAL
    
    TRISDbits.TRISD0=0; //OC1
    TRISDbits.TRISD1=0; //0C2
    TRISDbits.TRISD2=0; //0C3
    TRISDbits.TRISD3=0; //0C4
    
    //VAMOS A MANEJAR TODO CON EL MISMO TIMER, EL TIMER 2
    
    PR2=21591; //PERIODO DEL PWM (Tpwm/(TCY*PRESCALER))-1
    TMR2=0; // DONDE ARRANCA EL TIMER 2
    
    //PWM 1
    OC1R=0; //TON PARA EL PRIMER CICLO
    OC1RS=0;
    OC1CONbits.OCM=0b110;
    OC1CONbits.OCTSEL=0;
    
    //PWM 2
    OC2R=0; //TON PARA EL PRIMER CICLO
    OC2RS=0;
    OC2CONbits.OCM=0b110;
    OC2CONbits.OCTSEL=0;
    
    //PWM 2
    OC3R=0; //TON PARA EL PRIMER CICLO
    OC3RS=0;
    OC3CONbits.OCM=0b110;
    OC3CONbits.OCTSEL=0;
    
    //PWM 4
    OC4R=0; //TON PARA EL PRIMER CICLO
    OC4RS=0;
    OC4CONbits.OCM=0b110;
    OC4CONbits.OCTSEL=0;
    
    
    T2CONbits.T32=0; //SI QUEREMOS UN TIMER DE 32 O 16 BITS
    T2CONbits.TCKPS=0b00; //ELEGIMOS EL PRESCALER
    T2CONbits.TCS=0; //ELEGIMOS QUE QUEREMOS SEGUIR LAS INSTRUCCIONES INTERNAS COMO ORIGEN DEL TIMER
    T2CONbits.TON=1;//ENCENDEMOS EL TIMER2
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


