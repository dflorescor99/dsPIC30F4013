/*
 * File:   SPIslave.c
 * Author: root
 *
 * Created on November 27, 2020, 1:22 AM
 */



#include "xc.h"
#include <p30F4013.h>
#include <stdio.h>
#define MIPS 30


_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

void SPISLAVE();
void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void writestr(char str[20]);
void configuart();

int main(void) {
    char char_spi='\0';
    ADPCFG = 0b1111111111111111; 
    SPISLAVE();
    configuart();

    while (1) // repeat continuously

    {
        while(SPI1STATbits.SPIRBF==0);
        char_spi=SPI1BUF;
        U2TXREG=char_spi;
        while(U2STAbits.TRMT==1);
        U2TXREG='\n';
        while(U2STAbits.TRMT==1);
        U2TXREG='\r';
        while(U2STAbits.TRMT==1);
        while(SPI1STATbits.SPITBF);
        SPI1BUF='h';
        while(SPI1STATbits.SPITBF);
        
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



void configuart(void)
{
    OSCTUN=0; 
    RCONbits.SWDTEN=0;
    U2MODEbits.UARTEN = 0;// Disable UART
    U2MODEbits.STSEL = 0; // 1-stop bit
    U2MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U2MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U2BRG = 15;          //9600
    U2STAbits.UTXISEL = 0;         // Interrupcion cuando mandamos un caracter
    U2STAbits.URXISEL = 0;         // Interrupcion cuando recibimos un caracter
    U2MODEbits.UARTEN = 1;         // Enable UART
    U2STAbits.UTXEN = 1;           // Enable UART Tx, solo activamos el envio
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
