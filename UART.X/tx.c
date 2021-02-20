/*
 * File:   tx.c
 * Author: root
 *
 * Created on June 17, 2020, 7:47 PM
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

void configuart();
void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void writestr(char str[20]);

int main(void) 
{
    delay_us(100);
    configuart();
    IEC0bits.U1RXIE = 0;
    IEC0bits.U1TXIE = 0;
    char h[30];
    sprintf(h,"HOLA RICHARD\n");
    writestr(h);
    U1TXREG='\r';
    while (1) // repeat continuously

    {
        //sprintf(h,"HOLA RICHARD\n");
        writestr(h);
        //delay_us(500);
        U1TXREG='\r';
        //delay_us(500);
    }   
}

 void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt( void ) // la interrupcion por si queremos hacer algo, limpiamos la 
{                                                                    // el interrupt flag
    IFS0bits.U1RXIF=0;
    U1TXREG=U1RXREG;
}

void configuart(void)
{
    OSCTUN=0; 
    RCONbits.SWDTEN=0;
    U1MODEbits.UARTEN = 0;// Disable UART
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1BRG = 15;          //9600
    U1STAbits.UTXISEL = 0;         // Interrupcion cuando mandamos un caracter
    U1STAbits.URXISEL = 0;         // Interrupcion cuando recibimos un caracter
    U1MODEbits.UARTEN = 1;         // Enable UART
    U1STAbits.UTXEN = 1;           // Enable UART Tx, solo activamos el envio
}

void writestr(char str[20])
{
    int i=0;
    while (str[i]!='\0')
    {
        U1TXREG=str[i];
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