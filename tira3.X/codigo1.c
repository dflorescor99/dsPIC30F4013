/*
 * File:   codigo1.c
 * Author: root
 *
 * Created on July 5, 2020, 4:36 PM
 */


#include "xc.h"
#include <p33FJ256GP510.h>

unsigned int BufferA[32] __attribute__((space(dma)));
unsigned int BufferB[32] __attribute__((space(dma)));
unsigned int DmaBuffer = 0;

void ProcessADCSamples(unsigned int* ADCBuffer);

int main(void) {
    ADPCFG = 0b1111111111110000;
    
    AD1CON1bits.FORM = 3; 
    AD1CON1bits.SSRC = 2; 
    AD1CON1bits.ASAM = 1; 
    AD1CON1bits.AD12B = 0;
    AD1CON1bits.SIMSAM = 0;
    AD1CON2bits.BUFM = 0;
    AD1CON2bits.CSCNA = 1; 
    AD1CON2bits.CHPS = 0; 
    AD1CON3bits.ADRC = 0; 
    AD1CON3bits.ADCS = 63; 
    AD1CHS0bits.CH0SA = 0; 
    AD1CHS0bits.CH0NA = 0; 
    AD1CHS123bits.CH123SA = 0;
    AD1CHS123bits.CH123NA = 0; 
    AD1CSSH = 0x0000;
    AD1CSSL = 0x000F; 
    
    TMR3 = 0x0000;
    PR3 = 4999;
    IFS0bits.T3IF = 0; 
    IEC0bits.T3IE = 0;
    T3CONbits.TON = 1; 
    
    DMA5CONbits.AMODE = 0; 
    DMA5CONbits.MODE = 2; 
    DMA5PAD = (volatile unsigned int)&ADC1BUF0;
    DMA5CNT = 31; 
    DMA5REQ = 13; 
    DMA5STA = __builtin_dmaoffset(BufferA);
    DMA5STB = __builtin_dmaoffset(BufferB);
    IFS3bits.DMA5IF = 0; 
    IEC3bits.DMA5IE = 1; 
    DMA5CONbits.CHEN=1
    
    AD1CON1bits.ADDMABM = 0; 
    AD1CON2bits.SMPI = 3; 
    AD1CON4bits.DMABL = 3; 
    IFS0bits.AD1IF = 0; 
    IEC0bits.AD1IE = 0; 
    AD1CON1bits.ADON = 1; 
    
    while(1);
    return 0;
}

void __attribute__((interrupt, no_auto_psv)) _DMA5Interrupt(void)
{
    if(DmaBuffer == 0){
        ProcessADCSamples(BufferA);
    }
    else{
        ProcessADCSamples(BufferB);
    }
    DmaBuffer ^= 1;
    IFS3bits.DMA5IF = 0;
}

void ProcessADCSamples(unsigned int* ADCBuffer)
{
    //Procesamos el buffer
}