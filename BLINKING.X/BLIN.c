/*
 * File:   BLIN.c
 * Author: root
 *
 * Created on April 21, 2020, 1:08 PM
 */

void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);

#include "xc.h"
#include <p30F4013.h>
#define MIPS 30
#include "lcd.h"
#define B9 LATBbits.LATB9
#define B10 LATBbits.LATB10
#define B11 LATBbits.LATB11
#define B12 LATBbits.LATB12

_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

int main(void) 
{
    ADPCFGbits.PCFG2 = 1;
    ADPCFGbits.PCFG9 = 1;
    ADPCFGbits.PCFG10 = 1; //PONGO LOS PINES RB9-RB12 
    ADPCFGbits.PCFG11 = 1; // COMO DIGITALES
    ADPCFGbits.PCFG12 = 1;
    TRISBbits.TRISB9 =0;
    TRISBbits.TRISB10 = 0; 
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB12 = 0;
    TRISDbits.TRISD8 = 0;
    TRISDbits.TRISD9 = 0; 
    TRISDbits.TRISD2 = 0; 
    B9=0;
    while (1){
        delay_ms(1000);
        B9=1;
        delay_ms(1000);
        B10=1;
        delay_ms(1000);
        B11=1;
        delay_ms(1000);
        B12=1;
        delay_ms(1000);
        B9=0;
        delay_ms(1000);
        B10=0;
        delay_ms(1000);
        B11=0;
        delay_ms(1000);
        B12=0;
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


