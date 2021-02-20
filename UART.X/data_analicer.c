/*
 * File:   data_analicer.c
 * Author: root
 *
 * Created on September 9, 2020, 7:34 PM
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

void configuarttx();
void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void writestr(char str[20]);
void writeshex(uint16_t A);
void CN(void);
void ADC(void);

int main(void) 
{
    TRISBbits.TRISB11 = 0;
    //TRISBbits.TRISB4 = 0;
    CN();
    ADC();
    uint16_t AD=0;
    uint16_t AD1=0;  
    configuarttx();
    IEC0bits.U1RXIE = 0;
    IEC0bits.U1TXIE = 0;
    while (1) // repeat continuously

    {
        ADCON1bits.SAMP = 1;
        delay_us(1000); 	// for 100 mS  El tiempo de adquisición se maneja manual en // este ejemplo. Este Tad es enorme !!!!
        ADCON1bits.SAMP = 0; 		// start Converting
        while (!ADCON1bits.DONE); 	// conversion done? Espera a que DONE=1

            //ADC = ADCBUF0; 		// yes then descargo en la varible ADCValue el AD
            AD = ADCBUF0;
            //AD1 = ADCBUF1;
            writeshex(AD);
            //writeshex(AD1);
            //ADC=((ADC*4.75)/4095.0);
            //writeshex(ADC);
    }
    return 0;
}

void ADC()
{
    ADPCFG = 0b1111111111110111; //Seleccionamos AN3 como entrada analogica
    ADCON1 = 0x0000; 	// Register 18-1:
    ADCON2 = 0b0000000000000000;		// Register 18-2:
    //ADCON2 = 0b0000010000000100;
    ADCHS = 0b0000000000000011;
    //ADCHS = 0b0000000000000011;
    ADCSSL = 0;		// Register 18-6: ADCSSL: A/D Input Scan Select Register
    //ADCSSL = 0b0000000000011000;		// Register 18-6: ADCSSL: A/D Input Scan Select Register
    ADCON3 = 0x0002;           // Register 18-3: 
    
    ADCON1bits.ADON = 1;
}

void CN(void)
{
    CNEN1bits.CN4IE = 1; // Enable CN3 pin for interrupt detection
    CNPU1bits.CN4PUE=1;
    IEC0bits.CNIE = 1; // Enable CN interrupts
    IFS0bits.CNIF = 0; // Reset CN interrupt
}

void __attribute__ ((__interrupt__)) _CNInterrupt(void)
{
    IFS0bits.CNIF = 0;
    CNEN1bits.CN4IE = 0;
    if (~LATBbits.LATB2){
        delay_ms(200);
        B11=~B11;
        //delay_ms(200);
    }
    //delay_ms(200);
    CNEN1bits.CN4IE = 1;
}

void configuarttx(void)
{
    OSCTUN=0; 
    RCONbits.SWDTEN=0;
    U1MODEbits.UARTEN = 0;// Disable UART
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Pari vcxty, 8-data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1BRG = 15;           // UxBRG = Fcy/(16*BaudRate) - 1
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

void writeshex(uint16_t A)
{
    uint16_t mask=0x00FF;
    uint8_t FRAMEH=0x03;
    uint8_t FRAMEL=0xFC;
    uint8_t ADCLOW=mask&A;
    uint8_t ADCHIGH=mask&(A>>8);
    
    U1TXREG=FRAMEH;
    U1TXREG=ADCLOW;
    U1TXREG=ADCHIGH;
    //U1TXREG=ADCLOW;
    U1TXREG=FRAMEL;    
    delay_us(50);
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