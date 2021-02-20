/*
Prueba dsp
incluir dsp.h y libdsp-coff.a
RS232 a 115200 y Xtal de 8 MHZ usando rutina clock_usart.h
Para habilitar ventana SIM Uart1 : Debugger ----> Setting ----> Usat1 IO  click ENABLE
*/

#define MIPS 40

#include "p33FJ256GP510.h"
#include <stdio.h>
#include <string.h>
#include <dsp.h>
#include <time.h>
#include "clock_usart.h"
#include "terminal.h"
#include "delays.h"



//-------------------------------------   VER !!!
// Select Internal FRC at POR
_FOSCSEL(FNOSC_FRC);  	// FRC Oscillator
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT);
                         	// Clock Switching is enabled and Fail Safe Clock Monitor is disabled
                          	// OSC2 Pin Function: OSC2 is Clock Output
                          	// Primary Oscillator Mode: Disabled

_FWDT(FWDTEN_OFF);          	// Watchdog Timer Enabled/disabled by user software
                           	// (LPRC can be disabled by clearing SWDTEN bit in RCON register
//--------------------------------------------

int main (void)

	{

	clock_usart();
	clock_t start, stop;

	static double count=0;

	//TERM_reset;	// Hyperterminal like VT100
	//TERM_BACK_black;
	//TERM_FORE_green;
	//TERM_FORE_bright;
	//TERM_clear;
	//TERM_HOME;
    //printf("hola mundo\n");


	start = clock();

	printf("a");

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










