/*
 * File:   ADC_withoutDMA.c
 * Author: andres 2
 *
 * Created on 30 de abril de 2021, 03:55 AM
 */



#include "xc.h"
#include <p33FJ128GP802.h>
#include <stdio.h>
#include <stdlib.h>

// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)
#pragma config RBS = NO_RAM             // Boot Segment RAM Protection (No Boot RAM)

// FSS
#pragma config SWRP = WRPROTECT_OFF     // Secure Segment Program Write Protect (Secure segment may be written)
#pragma config SSS = NO_FLASH           // Secure Segment Program Flash Code Protection (No Secure Segment)
#pragma config RSS = NO_RAM             // Secure Segment Data RAM Protection (No Secure RAM)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Mode (Primary Oscillator (XT, HS, EC) w/ PLL)
#pragma config IESO = ON                // Internal External Switch Over Mode (Start-up device with FRC, then automatically switch to user-selected oscillator source when ready)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Source (XT Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow Only One Re-configuration)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler (1:32,768)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // POR Timer Value (128ms)
#pragma config ALTI2C = OFF             // Alternate I2C  pins (I2C mapped to SDA1/SCL1 pins)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)

#define FOSC 11059200*3
#define FCY FOSC/2
#define PERIODO 0.00395
#define PRESCALER 0
#define BAUDRATE 9600
#define BRGVAL ((FCY/BAUDRATE)/16)- 1
#define Nop()   __builtin_nop()

uint8_t a=0;
uint8_t b=0;
uint8_t c=0;
uint8_t d=0;


unsigned int BufferA[32] __attribute__((space(dma)));
unsigned int BufferB[32] __attribute__((space(dma)));
unsigned int DmaBuffer = 0;
void ProcessADCSamples(unsigned int* ADCBuffer);
void InitADC(void);
void InitT3(void);
void Init_UART1(void);
void WriteUART1(unsigned int data);
void InitClock();
void delay_us(unsigned int delay_count);
void delay_ms(unsigned long delay_count);
uint16_t ADCValue=0;
int main(void) 
{
    InitClock();
    Init_UART1();
    InitADC();
    AD1CON1bits.ADON = 1;
    while(1)
    {
        delay_ms(1);
        AD1CON1bits.SAMP = 0;
        while (!AD1CON1bits.DONE);// conversion done?
         // yes then get ADC value
        ADCValue=ADC1BUF0;
        WriteUART1(0x03);
        WriteUART1((ADCValue)&0x00FF);
        WriteUART1((ADCValue>>8)&0x00FF);
        WriteUART1(0xFC);
        
    }
    return 0;
}


void InitADC(void)
{
    TRISAbits.TRISA0=1;
//    PORTAbits.RA1=1;
//    PORTBbits.RB2=1;
//    PORTBbits.RB3=1;
    AD1PCFGL = 0b1111111111111110;  // all PORTB = Digital but RB7 = analog
    AD1CON1 = 0x0004;   // ASAM bit = 1 implies sampling ..
                              // starts immediately after last
                              // conversion is done
    AD1CON2bits.VCFG=0;
    AD1CHS0= 0b0000000000000000;    // Connect RB7/AN7 as CH0 input ..
                              // in this example RB7/AN7 is the input
    AD1CSSL = 0;
    AD1CON3bits.ADCS = 63;   // Sample time manual, Tad = internal 2 Tcy
    AD1CON2 = 0;
}


void Init_UART1()
{
    AD1PCFGL = 0xFFFF; // all pins as digital
    TRISA = 0;
    TRISB = 0;
    LATA = 0;
    LATB = 0;
    RPOR7bits.RP15R = 0b00011; //U1TX to RP15 = pin 26 
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U1MODEbits.ABAUD = 0; // Autobaud Disabled
    U1BRG = BRGVAL; // BAUD Rate Setting for 9600

    U1MODEbits.UARTEN = 1; // Enable UART
    U1STAbits.UTXEN = 1; // Enable Tx
    //IEC0bits.U1RXIE = 1; // enable UART1 receiver ISR
}

void WriteUART1(unsigned int data)
{
    while (U1STAbits.TRMT == 0);
    if (U1MODEbits.PDSEL == 3)
    {
        U1TXREG = data;
    }
    else
    {
        U1TXREG = data & 0xFF;
    }
}

void InitClock()
{
    PLLFBD = 10; // M = 12
    CLKDIVbits.PLLPOST = 0; // N2 = 2
    CLKDIVbits.PLLPRE = 0; // N1 = 2
    OSCTUN = 0;
    RCONbits.SWDTEN = 0;
}

void delay_us(unsigned int delay_count)
{
    delay_count = delay_count * FCY / 1000000;

    while (delay_count--);
}

void delay_ms(unsigned long delay_count)
{
    delay_count = delay_count * FCY / 1000;
    while (delay_count--);
}