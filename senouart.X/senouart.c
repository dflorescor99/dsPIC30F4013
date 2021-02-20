/*
 * File:   senouart.c
 * Author: root
 *
 * Created on May 18, 2020, 3:02 PM
 */

#include "xc.h"
#define MIPS 30
#include <p30F4013.h>		// si se cambia de DSC cambiar tb p30FXXXX.gld linker
#include <stdio.h>
#include <string.h>
#include <dsp.h>
#include <time.h>           // tic toc
#include <uart.h>


_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);    

#define DIV 1

fractional coef[5]={Q15(0.9921/DIV),Q15(-0.5/DIV),Q15(0.0313/DIV),Q15(0.0626/DIV),Q15(0.0313/DIV)};
fractional val[5] ={Q15(0.0),Q15(0.0),Q15(0.5),Q15(0.0),Q15(0.0)};
fractional sum[5];
fractional Y[1];
fractional dosY[1];
clock_t start, stop;
static double count=0;

#include "xc.h"

void configuart();
void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void writestr(char str[20]);

int main(void) {
    ADPCFG = 0xFFFB; 
    TRISBbits.TRISB9 =0;
    TRISBbits.TRISB10 = 0; 
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB12 = 0;
    TRISDbits.TRISD8 = 0;
    TRISDbits.TRISD9 = 0; 
    TRISDbits.TRISD2 = 0;
    ADCON1bits.ADON = 1;
    //delay_ms(2000); //esto solo es para encender un led de 
    //B9=1;           //verificacion
    //delay_ms(2000);
    //B9=0;
    configuart();
    IEC0bits.U1RXIE = 0;
    IEC0bits.U1TXIE = 1;
    char h[30];
    sprintf(h,"Se senso: %f \n",1000.00);
    writestr(h);
    U1TXREG='\r';
    U1TXREG='\n';
    U1TXREG='\r';
    while (1)

    {
        Y[0]=VectorDotProduct(5,val,coef);
        VectorAdd(1,dosY,Y,Y);
        val[1]=val[0];
        val[4]=val[3];
        val[3]=val[2];
        val[2]=0;
        val[0]=dosY[0];
        sprintf(h,"%f\n",(double)Fract2Float(dosY[0]));
        writestr(h);
        U1TXREG='\r';
        
    }
    return 0;
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt( void ) // la interrupcion por si queremos hacer algo, limpiamos la 
{                                                                    // el interrupt flag
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
    U1BRG = 194;          //9600
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
        delay_us(5000);
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
