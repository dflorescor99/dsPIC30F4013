/*
Prueba dsp en Dspic30f4013
Incluir dsp.h y libdsp-coff.a si se quiere utiizar el dsp...
...y ademas agregar un "heap" de 200 o 300 bytes en Projet properties...
...sino no compilara, aqui en este ejemplo ya estan agregado 300 bytes...
...RS232 a 56K y MIPS 30,  usando linbrería <uart.h>
Para habilitar ventana SIM Uart1 : Debugger ----> Setting ----> Usat1 IO  click ENABLE
El clock no está configurado, pero correra de cualquier manera en modo debugger
*/

#define MIPS 30

#include <p30F4013.h>		// si se cambia de DSC cambiar tb p30FXXXX.gld linker
#include <stdio.h>
#include <string.h>
#include <dsp.h>
#include <time.h>
#include <uart.h>



////-------------------------------------   VER stettings in CODE clock !!!
//// Select Internal FRC at POR
//_FOSCSEL(FNOSC_FRC);  	// FRC Oscillator
//_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT);
//                         	// Clock Switching is enabled and Fail Safe Clock Monitor is disabled
//                          	// OSC2 Pin Function: OSC2 is Clock Output
//                          	// Primary Oscillator Mode: Disabled
//
//_FWDT(FWDTEN_OFF);          	// Watchdog Timer Enabled/disabled by user software
//                           	// (LPRC can be disabled by clearing SWDTEN bit in RCON register
////--------------------------------------------





int main (void)
	{
// OpenUART1(U1MODEvalue, U1STAvalue, baud)//
unsigned int BRGvalue;			// ubrg		
unsigned int U1MODEvalue; 	// config1
unsigned int U1STAvalue;	// config2	

double baudrate,ERROR;

clock_t start, stop; 		//variables del tic toc
static double count=0.0;	//variables del tic toc


// ---------------------------------------------------------------------------------------------
// Configuracion parametros de la USART (U1MODEvalue, U1STAvalue y baud)  a xxxxxx bauds=bits /s
// Paginas 145/404 hasta 153/404 del manual pdf: dsPIC® Language Tools Libraries


//--------------------------------------------
//This contains the parameters to be configured in the
//UxMODE register as defined below:

U1MODEvalue = UART_EN & UART_IDLE_CON & 	// U1MODEvalue  Paginas 147/404 hasta 148 /404 dsPIC® Language Tools Libraries...
UART_DIS_WAKE & UART_EN_LOOPBACK &			// ... y el detalle de los registros está en pag 487/760 hasta 512/760    
UART_EN_ABAUD & UART_NO_PAR_8BIT &			// del dsPIC30F Familly Reference Manual
UART_1STOPBIT	;

//UART enable/disable
		//UART_EN  1111 1111 1110 0111
//UART_DIS

//UART Idle mode operation
		//UART_IDLE_CON
//UART_IDLE_STOP

//UART communication with ALT pins
//UART_ALTRX_ALTTX
//UART_RX_TX
//UART communication with ALT pins is available only for
//certain devices and the suitable data sheet should be
//referred to.

//UART Wake-up on Start
//UART_EN_WAKE
		//UART_DIS_WAKE

//UART Loopback mode enable/disable
		//UART_EN_LOOPBACK
//UART_DIS_LOOPBACK

//Input to Capture module
		//UART_EN_ABAUD
//UART_DIS_ABAUD

//Parity and data bits select
		//UART_NO_PAR_9BIT
//UART_ODD_PAR_8BIT
//UART_EVEN_PAR_8BIT
//UART_NO_PAR_8BIT

//Number of Stop bits
//UART_2STOPBITS
		//UART_1STOPBIT

//--------------------------------------------


//--------------------------------------------
//This contains the parameters to be configured in the
//U1STA register as defined below:


U1STAvalue = UART_INT_TX_BUF_EMPTY & 		// U1STAvalue  Paginas 147/404 hasta 148 /404 dsPIC® Language Tools Libraries...
UART_TX_PIN_NORMAL &						// ... y el detalle de los registros está en pag 487/760 hasta 512/760
UART_TX_ENABLE & UART_INT_RX_3_4_FUL &		// del dsPIC30F Familly Reference Manual
UART_ADR_DETECT_DIS &
UART_RX_OVERRUN_CLEAR	;

//UART Transmission mode interrupt select
		//UART_INT_TX_BUF_EMPTY
//UART_INT_TX

//UART Transmit Break bit
		//UART_TX_PIN_NORMAL
//UART_TX_PIN_LOW

//UART transmit enable/disable
		//UART_TX_ENABLE
//UART_TX_DISABLE

//UART Receive Interrupt mode select
//UART_INT_RX_BUF_FUL
		//UART_INT_RX_3_4_FUL
//UART_INT_RX_CHAR

//UART address detect enable/disable
//UART_ADR_DETECT_EN
		//UART_ADR_DETECT_DIS

//UART OVERRUN bit clear
		//UART_RX_OVERRUN_CLEAR

//--------------------------------------------


//--------------------------------------------
BRGvalue=32; // Si Fcy=30 MHz, RGG=32 ==> BAUD RATE=56K  (56.8182 K  +1.5% ERROR...
		// ...494/760 y 495/760 del dsPIC30F Familly Reference Manual
		// This is the value to be written into UxBRG register to set the baud rate.
//--------------------------------------------

// ---------------------------------------------------------------------------------------------





// ---------------------------------------------------------------------------------------------
//Configuracion aspecto de la old terminal VT100
//	TERM_reset;	// Hyperterminal like VT100
//	TERM_BACK_black;
//	TERM_FORE_green;
//	TERM_FORE_bright;
//	TERM_clear;
//	TERM_HOME;
// ---------------------------------------------------------------------------------------------

	OpenUART1(U1MODEvalue, U1STAvalue, BRGvalue);		// Activo la UART...

	start = clock();   // Imprimo Hola Milagros... y mido los TCY con tic toc

	// printf formateadores: https://es.cppreference.com/w/cpp/io/c/printf_format

	printf("\n\n\t\t\t ///Hola Milagros UART en Dspic30f4013///\r\n");
	Nop();

	stop = clock();

	count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY: %.1f\r\n\n",count);

	printf("UART_EN: %X\r\n",UART_EN);// asi puedo ver todos los bits del registro que "pisa" UART_EN
	printf("UART_1STOPBIT: %X\r\n",UART_1STOPBIT);// asi puedo ver todos los bits del registro que "pisa" UART_1STOPBIT

	printf("\nU1MODEvalue: %X\r\n",U1MODEvalue);// asi puedo ver todos los bits del registro que "pisa" U1MODEvalue
	printf("U1STAvalue: %X\r\n\n",U1STAvalue);// asi puedo ver todos los bits del registro que "pisa" U1STAvalue
	printf("BRGvalue: %d\r\n",BRGvalue);// asi puedo ver todos los bits del registro que "pisa" baud

	baudrate= 30000000.0 /( 16*(float)(BRGvalue+1)); // pag 495/760 del dsPIC30F Familly Reference Manual
	printf("Baud Rate= %f\r\n",baudrate);

	ERROR=((baudrate-56000.0)/56000.0)*100.0;
	printf("ERROR= %1.1f",ERROR);
	printf(" %%\r\n");
	printf("(Ver pag. 495/760 del dsPIC30F Familly Reference Manual)\r\n");


	while(1);  // Fin programa










//
//	while(1)
//	{
//	printf("aca esta el marker !\r\n");
//	}
//	printf("\r\nstart = %ld", start);
//	printf("\r\nstop = %ld", stop);
//
//
//	while(1);	// pie final aca te quedas...
	}










