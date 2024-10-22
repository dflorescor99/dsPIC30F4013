/*
 * File:   prueba_adc.c
 * Author: root
 *
 * Created on February 21, 2021, 2:33 AM
 */


// FOSC
#pragma config FOSFPR = FRC_PLL16       // Oscillator (FRC w/PLL 16x)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)
// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)
// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = BORV20          // Brown Out Voltage (Reserved)
#pragma config BOREN = PBOR_ON          // PBOR Enable (Enabled)
#pragma config MCLRE = MCLR_EN          // Master Clear Enable (Enabled)
// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)
// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

#include <p30F4013.h>
#include <stdio.h>
#include "xc.h"

#define MIPS 30


void conf_uart(void);
void conf_adc(void);
void conf_timer(void);
void send_uart(uint8_t A);
void writeshex(uint8_t A, uint16_t B, uint16_t C);
void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);

int main(void) {
    //RISBbits.TRISB2 = 1;
    uint16_t ADCValue0=0;
    uint16_t ADCValue1=0;
    uint8_t signal=0x00;
    conf_uart();
    conf_adc();
    conf_timer();
    //all PORTB = Digital; RB2 analog
    //SSRC bit = 010 implies GP TMR3
    //compare ends sampling and starts
    //converting.
    //Connect RB2/AN2 as CH0 input ..
    //in this example RB2/AN2 is the input
    // Sample time is TMR3, Tad = internal Tcy/2
    // Interrupcion despues de una conversion
    // set TMR3 to time out every 100 mSecs
    
    ADCON1bits.ADON = 1;
    ADCON1bits.ASAM = 1;
    while (1)
    {
        while (!IFS0bits.ADIF);
        ADCValue0 = ADCBUF0;
        ADCValue1 = ADCBUF1;
        signal=~signal;
        writeshex(signal,ADCValue0,ADCValue1);
        IFS0bits.ADIF = 0;
    }
    return 0;
}

void conf_uart(void)
{
    OSCTUN=0; 
    RCONbits.SWDTEN=0;
    U1MODEbits.UARTEN = 0;// Disable UART
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1BRG = 15;           // UxBRG = Fcy/(16*BaudRate) - 1, elegimos un Baudrate de 115200
    U1STAbits.UTXISEL = 0;         // Interrupcion cuando mandamos un caracter
    U1STAbits.URXISEL = 0;         // Interrupcion cuando recibimos un caracter
    U1MODEbits.UARTEN = 1;         // Enable UART
    U1STAbits.UTXEN = 1;           // Enable UART Tx, solo activamos el envio
}

void conf_adc(void)
{
    ADPCFG = 0b1111111111110011; //Ponemos todas las entradas como digitales menos AN2(RB2) y AN3(RB3)
    ADCON1 = 0b0000000001000000;
    ADCON3 = 0b0000000000010011;
    ADCON2 = 0b0000010000000100;
    ADCHS  = 0b0000000000000000;
    ADCSSL = 0b0000000000001100;

    //SSRC bit = 010 implies GP TMR3
    //compare ends sampling and starts
    //converting.
    //Connect RB2/AN2 as CH0 input ..
    //in this example RB2/AN2 is the input
    // Sample time is TMR3, Tad = internal Tcy/2
    // Interrupcion despues de una conversion
    // set TMR3 to time out every 100 mSecs
    
}

void conf_timer(void)
{
    TMR3 = 0;
    PR3= 46874;
    T3CON = 0b1000000000100000;
}

void send_uart(uint8_t A)
{    
    U1TXREG=A;
    delay_us(50);
}

void writeshex(uint8_t A, uint16_t B, uint16_t C)
{
    uint16_t mask=0x00FF;
    uint8_t FRAMEH=0x03;
    uint8_t FRAMEL=0xFC;
    uint8_t ADCLOW1=mask&B;
    uint8_t ADCHIGH1=mask&(B>>8);
    uint8_t ADCLOW=mask&C;
    uint8_t ADCHIGH=mask&(C>>8);
    
    U1TXREG=FRAMEH;
    delay_us(50);
    U1TXREG=A;
    delay_us(50);
    U1TXREG=ADCLOW1;
    delay_us(50);
    U1TXREG=ADCHIGH1;
    delay_us(50);
    U1TXREG=ADCLOW;
    delay_us(50);
    U1TXREG=ADCHIGH;
    delay_us(50);
    U1TXREG=FRAMEL;
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