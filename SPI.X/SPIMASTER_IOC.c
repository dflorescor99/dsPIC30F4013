/*
 * File:   SPIMASTER_IOC.c
 * Author: root
 *
 * Created on January 17, 2021, 8:45 PM
 */


#include "xc.h"
#include <p30F4013.h>
#include <stdio.h>
#define MIPS 30
#define B11 LATBbits.LATB11

_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

void SPIMASTER();
void CN(void);
void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void writestr(char str[20]);

int main(void) {

    ADPCFG = 0b1111111111111111; 
    TRISBbits.TRISB11 = 0;
    SPIMASTER();
    CN();
    //delay_ms(100);
    //char recive = '\0';
    while (1) // repeat continuously

    {
        
    }   
    return 0;
}

void __attribute__ ((interrupt,auto_psv)) _CNInterrupt(void)
{
    IFS0bits.CNIF = 0;
    CNEN1bits.CN4IE = 0;
    CNEN1bits.CN5IE = 0;
    if (PORTBbits.RB2==0){
        while(SPI1STATbits.SPITBF);
        delay_us(100);
        B11=1;
        SPI1BUF='E';
    }
    
    else if (PORTBbits.RB3==0){
        while(SPI1STATbits.SPITBF);
        delay_us(100);
        B11=0;
        SPI1BUF='A';
    }
    delay_ms(100);
    CNEN1bits.CN4IE = 1;
    CNEN1bits.CN5IE = 1;
}

void SPIMASTER()
{
    SPI1CONbits.MSTEN=1;     //DEFINIMOS COMO MAESTRO
    SPI1CONbits.MODE16=0;    //DEFINIMOS SI QUEREMOS 16 BITS O 8 BITS
    SPI1CONbits.CKE=0;       //LA TRANSISION DE DATOS DE DA CUANDO EL CLOCK PASA DE IDLE A ACTIVE 
    SPI1CONbits.CKP=0;       //DEFINIMOS QUE ES ACTIVO E IDLE, EN ESTE CASO ACTIVE ES 5V E IDLE 0V 
    SPI1CONbits.SSEN=0;      //PULSO DE SINCRONIZACION (SS) ESTA DESACTIVADO
    SPI1CONbits.SPRE=0b110;  //(0b000->1:8...0b111->1:1)            SECONDARY PRESCALER    FSCK=    FCY
    SPI1CONbits.PPRE=0b00;   //0b00->1:64..0b01->1:16..0b11->1:1)  PRIMARY PRESCALER            (SPRE)*(PPRE)
    SPI1STATbits.SPIEN=1;    //HABILITAMOS EL MODULO SPI
    IFS0bits.SPI1IF=0;
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


void writestr(char str[20])
{
    int i=0;
    while (str[i]!='\0')
    {
        U2TXREG=str[i];
        delay_us(50);
        i++;
    }
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
