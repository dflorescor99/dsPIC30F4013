/*
 * File:   uart.c
 * Author: root
 *
 * Created on May 29, 2020, 4:46 PM
 */


#include "xc.h"
#include <p30F4013.h>
#include <stdio.h>
#define MIPS 30
_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);
void configuart();
void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void writestr(char str[20]);
int main(void)
{
configuart();
IEC0bits.U1RXIE = 0;
IEC0bits.U1TXIE = 1;
char h[30];
sprintf(h,"Hola Richard");
while (1) // repeat continuously
{
    writestr(h);
    U1TXREG='\n';
    //delay_us(5000);
    U1TXREG='\r';
    //delay_us(5000);
    U1TXREG='\n';
    //delay_us(5000);

 }
return 0;
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt( void ) // la interrupcion por si queremos hacer algo, limpiamos la
{                                                                   // el interrupt flag
    IFS0bits.U1TXIF=0;
}

void configuart(void)
{
    OSCTUN=0;
    RCONbits.SWDTEN=0;
    U1MODEbits.UARTEN = 0;// Disable UART
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1BRG = 194; //9600
    U1STAbits.UTXISEL = 0; // Interrupcion cuando mandamos un caracter
    U1STAbits.URXISEL = 0; // Interrupcion cuando recibimos un caracter
    U1MODEbits.UARTEN = 1; // Enable UART
    U1STAbits.UTXEN = 1; // Enable UART Tx, solo activamos el envio
}
void writestr(char str[20])
{
    int i=0;
    while (str[i]!='\0')
    {
        U1TXREG=str[i];
        delay_us(600);
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