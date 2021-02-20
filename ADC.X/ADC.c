/*
 * File:   ADC.c
 * Author: root
 *
 * Created on April 24, 2020, 7:12 PM
 */



void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);

#include "xc.h"
#include <p30F4013.h>
#define MIPS 30
#define B9 LATBbits.LATB9
#define B10 LATBbits.LATB10
#define B11 LATBbits.LATB11
#define B12 LATBbits.LATB12

_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

int main(void) 
{
    int ADCValue = 0;
    ADPCFG = 0b1111111111110111; 
    TRISBbits.TRISB9 =0;
    TRISBbits.TRISB10 = 0; 
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB12 = 0;
    TRISDbits.TRISD8 = 0;
    TRISDbits.TRISD9 = 0; 
    TRISDbits.TRISD2 = 0;
    B9=1;
    delay_ms(1000);
    B9=0;
    delay_ms(1000);
    B9=1;
    delay_ms(1000);
    B9=0;
    delay_ms(1000);
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
        
        if (((ADC)/4095.0)<0.25){
            B9=1;
            B10=0;
            B11=0;
            B12=0;
        }
        else if (((ADC)/4095.0)<0.50){
            B9=1;
            B10=1;
            B11=0;
            B12=0;
        }
        else if (((ADC)/4095.0)<0.75){
            B9=1;
            B10=1;
            B11=1;
            B12=0;
        }
        else {
            B9=1;
            B10=1;
            B11=1;
            B12=1;
        }
        
    }
    return 0;
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