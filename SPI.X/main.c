/*
 * File:   main.c
 * Author: root
 *
 * Created on January 18, 2021, 1:23 AM
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


int main(void) {
    return 0;
}
