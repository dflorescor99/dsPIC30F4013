/*
 * File:   SPISLAVE_IOC.c
 * Author: root
 *
 * Created on January 17, 2021, 8:56 PM
 */


#include "xc.h"
#include <p30F4013.h>
#include <stdio.h>
#define MIPS 30
#define B11 LATBbits.LATB11


_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

void SPISLAVE();
void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);

int main(void) {
    char char_spi='\0';
    ADPCFG = 0b1111111111111111; 
    TRISBbits.TRISB11 = 0;
    SPISLAVE();

    while (1) // repeat continuously

    {
        while(SPI1STATbits.SPIRBF==0);
        char_spi=SPI1BUF;
        if (char_spi=='E'){
            B11=1;
        }
        else if (char_spi=='A'){
            B11=0;
        }
        delay_ms(50);
        
    }   
    return 0;
}

void SPISLAVE()
{
    SPI1CONbits.MSTEN=0;     //DEFINIMOS COMO ESCLAVO
    SPI1CONbits.MODE16=0;    //DEFINIMOS SI QUEREMOS 16 BITS O 8 BITS
    SPI1CONbits.CKE=0;       //LA TRANSISION DE DATOS DE DA CUANDO EL CLOCK PASA DE IDLE A ACTIVE 
    SPI1CONbits.CKP=0;       //DEFINIMOS QUE ES ACTIVO E IDLE, EN ESTE CASO ACTIVE ES 5V E IDLE 0V 
    SPI1CONbits.SSEN=0;      //PULSO DE SINCRONIZACION (SS) ESTA DESACTIVADO
    SPI1CONbits.SPRE=0b110;  //(0b000->1:8...0b111->1:1)            SECONDARY PRESCALER    FSCK=    FCY
    SPI1CONbits.PPRE=0b00;   //0b00->1:64..0b01->1:16..0b11->1:1)  PRIMARY PRESCALER            (SPRE)*(PPRE)
    SPI1STATbits.SPIEN=1;    //HABILITAMOS EL MODULO SPI
    IFS0bits.SPI1IF=0;
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
