/*
Prueba dsp
incluir dsp.h y libdsp-coff.a
RS232 a 115200 y Xtal de 8 MHZ usando rutina clock_usart.h
*/

#define MIPS 40

#include <p33fj256gp510.h>
#include <stdio.h>
#include <string.h>
#include <dsp.h>
#include <time.h>
#include "clock_usart.h"
#include "terminal.h"
#include "delays.h"

// Select Internal FRC at POR
_FOSCSEL(FNOSC_FRC);  	// FRC Oscillator
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT);
                         	// Clock Switching is enabled and Fail Safe Clock Monitor is disabled
                          	// OSC2 Pin Function: OSC2 is Clock Output
                          	// Primary Oscillator Mode: Disabled

_FWDT(FWDTEN_OFF);          	// Watchdog Timer Enabled/disabled by user software
                           	// (LPRC can be disabled by clearing SWDTEN bit in RCON register

//double minimo;
unsigned int i=0;
volatile unsigned int j=0;
volatile unsigned int k=0;
unsigned long int ppp;
clock_t start, stop;

void pepe(int pp);

int main (void)

	{
	clock_usart();

	printf("\n\t\t\t ///Prueba timers///\r\n\n");
	Nop();
 
	TERM_reset;	// Hyperterminal like VT100
	TERM_BACK_black;	TERM_FORE_green;	TERM_FORE_bright;	TERM_clear;	TERM_HOME;

				// Seteo Timer 1
	T1CONbits.TON = 0; 		// Disable Timer
	T1CONbits.TCS = 0; 		// Select internal instruction cycle clock
	T1CONbits.TGATE = 0; 	// Disable Gated Timer mode
	T1CONbits.TCKPS = 0b10; 	// Select 00=1; 01=8; 10=64; 11=256
	TMR1 = 0x00; 		// Clear timer register
	PR1 = 60000; 		// Load the period value
	IPC0bits.T1IP = 0x01; 	// Set Timer1 Interrupt Priority Level
	IFS0bits.T1IF = 0; 		// Clear Timer1 Interrupt Flag
	IEC0bits.T1IE = 1; 		// Enable Timer1 interrupt
	T1CONbits.TON = 1; 		// Start Timer

				// Seteo Timer 3
	T3CONbits.TON = 0; // Stop any 16-bit Timer3 operation
	T2CONbits.TON = 0; // Stop any 16/32-bit Timer3 operation
	T2CONbits.T32 = 1; // Enable 32-bit Timer mode
	T2CONbits.TCS = 0; // Select internal instruction cycle clock
	T2CONbits.TGATE = 0; // Disable Gated Timer mode
	T2CONbits.TCKPS = 0b11;// Select 1:1 Prescaler
	TMR3 = 0x00; // Clear 32-bit Timer (msw)
	TMR2 = 0x00; // Clear 32-bit Timer (lsw)
	PR3 = 0xF000; // Load 32-bit period value (msw)
	PR3 = 0x0000; // Load 32-bit period value (lsw)
	IPC2bits.T3IP = 0x01; // Set Timer3 Interrupt Priority Level
	IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
	IEC0bits.T3IE = 1; // Enable Timer3 interrupt
	T2CONbits.TON = 1; // Start 32-bit Timer


	while(1)
		{
		ppp=100000;
		TERM_clear;
		TERM_HOME;
		
		printf("prueba:%6u  %6u   %6u\r",i++,j,k);
		printf("\r\ndelta = %ld", stop-start);
		printf("\r\nrºr");
		
		pepe(20);
		
		while(ppp--); // demora
		}

	while(1);	// pie final por las dudas

	start = clock();
	stop = clock()-19;
	}


void pepe(int pp)
	{
	printf("\n\n\r");
	while((pp--))
		{
		printf(">");
		}
	Nop();
	}

	/* Example code for Timer1 ISR*/
void __attribute__((__interrupt__, __shadow__,__auto_psv__)) _T1Interrupt(void)
	{
	/* Interrupt Service Routine code goes here */

	j++;

	IFS0bits.T1IF = 0; //Clear Timer1 interrupt flag
	}


	/* Example code for Timer3 ISR*/
void __attribute__((__interrupt__, __shadow__,__auto_psv__)) _T3Interrupt(void)
	{
	/* Interrupt Service Routine code goes here */
	k++;

	IFS0bits.T3IF = 0; //Clear Timer3 interrupt flag
	}







