/*
 * File:   ADC_DMA.c
 * Author: andres 2
 *
 * Created on 30 de abril de 2021, 08:45 AM
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

uint16_t ADCValue0=0;
uint16_t ADCValue1=0;
uint16_t ADCValue2=0;
uint16_t ADCValue3=0;


unsigned int BufferA[4] __attribute__((space(dma))); //INICIALIZAMOS LOS BUFFER, CADA UNO TIENE EL TAMAÑO
unsigned int BufferB[4] __attribute__((space(dma))); //DE CUANTAS MUESTRAS TENEMOS

unsigned int DmaBuffer = 0; //UNA VARIABLE AUXILIAR PARA SABER QUE BUFFER ESTA LISTO
uint16_t mask=0x00FF;

void ProcessADCSamples(unsigned int* ADCBuffer);
void InitADC(void);
void InitT3(void);
void InitDMA5(void);
void Init_UART1(void);
void WriteUART1(unsigned int data);
void InitClock();

int main(void) 
{
    InitClock();
    Init_UART1();
    InitADC();
    InitT3();
    InitDMA5();
    IFS3bits.DMA5IF = 0;
    IEC3bits.DMA5IE = 1;
    AD1CON1bits.ADON = 1;
    while(1)
    {
        //WriteUART1(0xFF);
    }
    return 0;
}
void __attribute__((interrupt, no_auto_psv)) _DMA5Interrupt(void)
{
    IFS3bits.DMA5IF = 0;
    //WriteUART1(0x00);
    if(DmaBuffer == 0){
        ProcessADCSamples(BufferA); //PROCESAMOS LAS MUESTRAS DEL BUFFER
    }
    else{
        ProcessADCSamples(BufferB);
    }
    DmaBuffer ^= 1;
}
void ProcessADCSamples(unsigned int* ADCBuffer)
{
    ADCValue0=ADCBuffer[0];
    ADCValue1=ADCBuffer[1];
    ADCValue2=ADCBuffer[2];
    ADCValue3=ADCBuffer[3];
    WriteUART1(0x03);

    WriteUART1(ADCValue0&mask);
    WriteUART1((ADCValue0>>8)&mask);
    WriteUART1(ADCValue1&mask);
    WriteUART1((ADCValue1>>8)&mask);
    WriteUART1(ADCValue2&mask);
    WriteUART1((ADCValue2>>8)&mask);
    WriteUART1(ADCValue3&mask);
    WriteUART1((ADCValue3>>8)&mask);
    
    WriteUART1(0xFC);
}

void InitADC(void)
{
    TRISAbits.TRISA0=1; // IMPORTANTE DEFINIR LOS PUERTOS COMO ENTRADAS!!
    TRISAbits.TRISA1=1; //
    TRISBbits.TRISB2=1; //
    TRISBbits.TRISB3=1; //
    ADPCFG = 0b1111111111001100;//DEFINIMOS COMO PUERTOS ANALOGICOS LOS PINES A USAR
    AD1CON1bits.FORM = 0;
    AD1CON1bits.SSRC = 2;//ELEGIMOS EL TIMER 3 COMO FUENTE DE CLOCK
    AD1CON1bits.ASAM = 1;
    AD1CON1bits.AD12B = 0; //MODO 10 BITS
    AD1CON1bits.SIMSAM = 0;
    AD1CON2bits.VCFG = 0;
    AD1CON2bits.BUFM = 0;
    AD1CON2bits.CSCNA = 1;//CHANEL SCANNING MODE
    AD1CON2bits.CHPS = 0;
    AD1CON3bits.ADRC = 0;
    AD1CON3bits.ADCS = 63;//CUANTOS TCY ES UN TAD
    AD1CHS0bits.CH0SA = 0;
    AD1CHS0bits.CH0NA = 0;
    AD1CHS123bits.CH123SA = 0;
    AD1CHS123bits.CH123NA = 0;
    AD1CSSL = 0b0000000000110011; //PARA EL MUESTREO SECUENCIAL ELEGIMOS A ESOS CANALES
    AD1CON1bits.ADDMABM = 0;//ELEGIMOS USAR DMA
    AD1CON2bits.SMPI = 3;
    AD1CON4bits.DMABL = 0;//CUANTAS PALABRAS EN EL BUFFER POR CADA ENTRADA ANALOGICA
    IFS0bits.AD1IF = 0; //NO USAMOS LA INTERRUPCION DEL ADC
    IEC0bits.AD1IE = 0; //
}

void InitT3(void)
{
    TMR3 = 0x0000;
    PR3 = 2499;;
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 0;
    T3CONbits.TON = 1;
}

void InitDMA5(void)
{
    DMA5CONbits.SIZE = 0;
    DMA5CONbits.AMODE = 0;//
    DMA5CONbits.MODE = 2; //MODO CONTINUO EN PING PONG, MIENTRAS SE LLENA UNO SE OPERA EL OTRO
    DMA5PAD = (volatile unsigned int)&ADC1BUF0;// LA DIRECCION DE MEMORIA DONDE APUNTA, DE ESTA DIRECCION TOMARA LOS DATOS
    DMA5CNT = 3; //DESPUES DE CUANTAS TRANSFERENCIAS HAY UNA INTERRUPCION,DMA5CNT+1 ES EL NUMERO DE TRANSFERENCIAS QUE SE REALIZARAN
    DMA5REQ = 13;
    DMA5STA = __builtin_dmaoffset(BufferA);//ES LA DIRECCION DEL BUFFER DONDE SE ALMACENAN
    DMA5STB = __builtin_dmaoffset(BufferB);//
    DMA5CONbits.CHEN = 1;//SE ACTIVA EL CANAL
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