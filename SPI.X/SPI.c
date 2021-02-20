/*
 * File:   SPI.c
 * Author: root
 *
 * Created on September 26, 2020, 1:10 AM
 */

#include "xc.h"
#include <p30F4013.h>
#include <stdio.h>

#define TFT_DIN PORTFbits.RF2
#define TFT_SCK PORTFbits.RF6

#define ST7789_HARD_SPI2


#define TFT_RST  LATDbits.LATD0
#define TFT_DC LATDbits.LATD1
#define TFT_RST_DIR   TRISDbits.TRISD0
#define TFT_DC_DIR    TRISDbits.TRISD1

#define MIPS 30

void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);

#include "ST7789.h"
#include "GFX_Library.h"

_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

int main(void) {
    ADPCFG = 0b1111111111111111; 
    
    SPI1CONbits.MSTEN=1;
    SPI1CONbits.MODE16=1;
    SPI1CONbits.CKE=0;
    SPI1CONbits.CKP=0;
    SPI1CONbits.SSEN=0;
    SPI1CONbits.SPRE=0b110;
    SPI1CONbits.PPRE=0b00;
    SPI1STATbits.SPIEN=1;
    IFS0bits.SPI1IF=0;
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
