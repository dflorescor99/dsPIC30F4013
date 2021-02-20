/*
 * File:   4013.c
 * Author: root
 *
 * Created on April 12, 2020, 4:17 PM
 */

    
#include "xc.h"
#include "p30F4013.h"
#define MIPS 30
#include <stdio.h>
#include <string.h>
#include <dsp.h>
#include <time.h>
#include "clock_usart.h"
#include "terminal.h"
#include "delays.h"


//-------------------------------------   VER !!!
// Select Internal FRC at POR
_FOSC(FRC_PLL16);
 

_FWDT(WDT_OFF);          	// Watchdog Timer Enabled/disabled by user software
                           	// (LPRC can be disabled by clearing SWDTEN bit in RCON register
//--------------------------------------------

int main (void)

	{
    RCONbits.SWDTEN=0; //apagamos el watchdog timer
	//clock_usart();
	clock_t start, stop;
	static double count=0;

	//TERM_reset;	// Hyperterminal like VT100
	//TERM_BACK_black;
	//TERM_FORE_green;
	//TERM_FORE_bright;
	//TERM_clear;
	//TERM_HOME;
    printf("hola mundo\n");


	start = clock(); 
	printf("\n\n\nxdxd\n\n\n");
	Nop();

	stop = clock();

	count=(stop-start)-19.0;  // 19 TCY start clock
	printf("TCY: %.1f\r\n\n",count);

	while(1);



	while(1)
	{
	printf("aca esta el marker !\r\n");
	}
	printf("\r\nstart = %ld", start);
	printf("\r\nstop = %ld", stop);


	while(1);	// pie final aca te quedas...
	}

