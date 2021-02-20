/*
 * File:   newmainXC16pwmadc.c
 * Author: root
 *
 * Created on May 29, 2020, 8:58 PM
 */


void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void pwm_conf(void);

#include "xc.h"
#include <p30F4013.h>
#define MIPS 1.84


_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

int main(void) 
{
    pwm_conf();
    OC1RS=100; // TON DEL PWM
    int ADCValue = 0;
    ADPCFG = 0b1111111111110111;
    ADCON1 = 0x0000;
    ADCHS = 0b0000000000000011;
    ADCSSL = 0;
    ADCON3 = 0x0002;
    ADCON2 = 0;
    ADCON1bits.ADON = 1;
    while (1) // repeat continuously

    {
        ADCON1bits.SAMP = 1;
        delay_ms(100); 	// for 100 mS  El tiempo de adquisici  ón se maneja manual en // este ejemplo. Este Tad es enorme !!!!
        ADCON1bits.SAMP = 0; 		// start Converting
    while (!ADCON1bits.DONE); 	// conversion done? Espera a que DONE=1

        ADCValue = ADCBUF0; 		// yes then descargo en la varible ADCValue el ADCBUF0
        float ADC = 0.0;
        ADC = (float)ADCValue;
        
        if (((ADC)/4095.0)<0.1){
            OC1RS=0;
        }
        else if (((ADC)/4095.0)<0.2){
            OC1RS=200;
        }
        else if (((ADC)/4095.0)<0.3){
            OC1RS=200;
        }
        else if (((ADC)/4095.0)<0.4){
            OC1RS=300;
        }
        else if (((ADC)/4095.0)<0.5){
            OC1RS=400;
        }
        else if (((ADC)/4095.0)<0.6){
            OC1RS=500;
        }
        else if (((ADC)/4095.0)<0.7){
            OC1RS=600;
        }
        else if (((ADC)/4095.0)<0.8){
            OC1RS=700;
        }
        else if (((ADC)/4095.0)<0.9){
            OC1RS=800;
        }
        else {
            OC1RS=900;
        }
        
    }
}

void pwm_conf(void)
{
    TRISDbits.TRISD0=0;
    PR2=1000; //PERIODO DEL PWM (Tpwm/(TCY*PRESCALER))-1
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


