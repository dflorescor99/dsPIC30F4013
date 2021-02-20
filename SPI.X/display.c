/*
 * File:   display.c
 * Author: root
 *
 * Created on January 17, 2021, 11:57 PM
 */

#define MIPS 30
#include "xc.h"
#include <p30F4013.h>
#include <stdio.h>
#define TFT_DC LATBbits.LATB11
#define TFT_RST LATBbits.LATB12
#include "ST7789_HONDU.h"
//#include "GFX_Library.h"

_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF); 

void SPIMASTER();
void CN(void);



int main(void) {

    ADPCFG = 0b1111111111111111; 
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB12 = 0;
    TFT_DC=1;
    TFT_RST=1;
    SPIMASTER();
    while (1) // repeat continuously

    {
        // initialize the ST7789 display
        tft_init();
 
        // fill screen with black
        fillScreen(ST7789_WHITE);
        while(1)
        {
            fillScreen(ST7789_WHITE);
        }
 
    }   
    return 0;
}


void SPIMASTER()
{
    SPI1CONbits.MSTEN=1;     //DEFINIMOS COMO MAESTRO
    SPI1CONbits.MODE16=0;    //DEFINIMOS SI QUEREMOS 16 BITS O 8 BITS
    SPI1CONbits.CKE=0;       //LA TRANSISION DE DATOS DE DA CUANDO EL CLOCK PASA DE IDLE A ACTIVE 
    SPI1CONbits.CKP=0;       //DEFINIMOS QUE ES ACTIVO E IDLE, EN ESTE CASO ACTIVE ES 5V E IDLE 0V 
    SPI1CONbits.SSEN=0;      //PULSO DE SINCRONIZACION (SS) ESTA DESACTIVADO
    SPI1CONbits.SPRE=0b100;  //(0b000->1:8...0b111->1:1)            SECONDARY PRESCALER    FSCK=    FCY
    SPI1CONbits.PPRE=0b00;   //0b00->1:64..0b01->1:16..0b11->1:1)  PRIMARY PRESCALER            (SPRE)*(PPRE)
    SPI1STATbits.SPIEN=1;    //HABILITAMOS EL MODULO SPI
    IFS0bits.SPI1IF=0;
}



