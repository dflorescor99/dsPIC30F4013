/*
 * File:   interrupt2.c
 * Author: root
 *
 * Created on January 17, 2021, 2:32 AM
 */


#include "xc.h"
#include <p30F4013.h>
#include <stdio.h>
#define MIPS 30
#define B11 LATBbits.LATB11

_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

unsigned int PORTB4=0;

void CN(void);
void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);

int main(void) {
    ADPCFG = 0b1111111111111111; 
    TRISBbits.TRISB11 = 0;
    //TRISBbits.TRISB2 = 0;
    //TRISBbits.TRISB3 = 0;
    CN();
    B11=1;
    //delay_ms(2000);
    while(1);
    return 0;
}

void __attribute__ ((interrupt,auto_psv)) _CNInterrupt(void)
{
    IFS0bits.CNIF = 0;
    CNEN1bits.CN4IE = 0;
    CNEN1bits.CN5IE = 0;
    if (PORTBbits.RB2==0){
        B11=1;
        //delay_ms(200);
    }
    
    else if (PORTBbits.RB3==0){
        B11=0;
        //delay_ms(200);
    }
    delay_ms(50);
    CNEN1bits.CN4IE = 1;
    CNEN1bits.CN5IE = 1;
}

void CN(void)
{
    CNEN1bits.CN4IE = 1; // HABILITAMOS CN4 PARA INTERRUPCIONES
    CNEN1bits.CN5IE = 1; // HABILITAMOS CN5 PARA INTERRUPCIONES
    CNPU1bits.CN4PUE=1;
    CNPU1bits.CN5PUE=1;
    IEC0bits.CNIE = 1; // Enable CN interrupts
    IFS0bits.CNIF = 0; // Reset CN interrupt
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
