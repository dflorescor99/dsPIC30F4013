/*
Prueba dsp en Dspic30f4013
Incluir dsp.h y libdsp-coff.a si se quiere utiizar el dsp...
...y ademas agregar un "heap" de 200 o 300 bytes en Projet properties...
...sino no compilara, aqui en este ejemplo ya estan agregado 300 bytes...
...RS232 a 56K y MIPS 30,  usando linbrería <uart.h>
... el ejemplo esta como el manual, debería quitarse UART_EN_LOOPBACK y UART_EN_ABAUD
Para habilitar ventana SIM Uart1 : Debugger ----> Setting ----> Usat1 IO  click ENABLE
El clock no está configurado, pero correra de cualquier manera en modo debugger
*/

#define MIPS 30

 

#include <p30F4013.h>		// si se cambia de DSC cambiar tb p30FXXXX.gld linker
#include <stdio.h>
#include <string.h>
#include <dsp.h>
#include <time.h>           // tic toc
#include <uart.h>
#include "terminal.h"		// hiperterminal colores
#include "delays.h"


////-------------------------------------  

// Ver dspic30_device_conf.pdf que esta en el zip de proyecto y configuration bits en el MPLAB...Configure... Configuration Bits 
//• FOSC: Oscillator Configuration Register (Note 2) (0xF80000)
//• FWDT: Watchdog Timer Configuration Register (0xF80002)
//• FBORPOR: BOR and POR Configuration Register (0xF80004)
//• FBS: Boot Segment Configuration Register (0xF80006)
//• FSS: Secure Segment Configuration Register (0xF80008)
//• FGS: General Segment Configuration Register (0xF8000A)
//• FICD: In-Circuit Debugger Configuration Register (0xF8000C)
//  The device Configuration registers can be programmed using Run-Time...
//  ...Self-Programming(RTSP), In-Circuit Serial Programming™ (ICSP™), or by a device programmer.


//Macros for Configuration Fuse Registers:
//Invoke macros to set up  device configuration fuse registers.
//The fuses will select the oscillator source, power-up timers, watch-dog
//timers, BOR characteristics etc. The macros are defined within the device
//header files. The configuration fuse registers reside in Flash memory.
_FOSC(CSW_FSCM_OFF & XT_PLL16);  //Run this project using an external crystal
                                //routed via the PLL in 16x multiplier mode
                                //For the 7.3728 MHz crystal we will derive a
                                //throughput of 7.3728e+6*16/4 = 29.4 MIPS(Fcy)
                                //,~33.9 nanoseconds instruction cycle time(Tcy).
_FWDT(WDT_OFF);                 //Turn off the Watch-Dog Timer.
_FBORPOR(MCLR_EN & PWRT_OFF);   //Enable MCLR reset pin and turn off the
                                //power-up timers.
_FGS(CODE_PROT_OFF);            //Disable Code Protection

////--------------------------------------------




int main (void)
	{
// OpenUART1(U1MODEvalue, U1STAvalue, baud)//
		
unsigned int U1MODEvalue; 	// config1
unsigned int U1STAvalue;	// config2
unsigned int BRGvalue;		// ubrg	(config 3)

double baudrate,ERROR,Tiempoej;      // variables double 

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


U1STAvalue = UART_INT_TX_BUF_EMPTY & 		// U1STAvalue  Paginas 144/404 hasta 144 /404 dsPIC® Language Tools Libraries...
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
		// ...494/760 y 495/760 y 496/760 del dsPIC30F Familly Reference Manual
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

	OpenUART1(U1MODEvalue, U1STAvalue, BRGvalue);		// Activo la UART 56000,N,1...

	start = clock();   // Imprimo Hola Milagros... y mido los TCY con tic toc

	// printf formateadores: https://es.cppreference.com/w/cpp/io/c/printf_format

	printf("\n\n\t\t\t ///Hola Hondu, CLOCK en Dspic30f4013///\r\n\n");
	Nop();

//	stop = clock();
//	count=(stop-start)-19.0;  // 19 TCY start clock
//	printf("# TCY: %.1f\r\n\n",count);




										// Analisis de la conf del Clock
//-----Registros asociados al clock

	printf("Veamos el valor de CSW_FSCM_OFF: %X\r\n",CSW_FSCM_OFF);
	printf("Veamos el valor de XT_PLL16: %X\r\n",XT_PLL16);
	printf("Veamos el valor de WDT_OFF: %X\r\n",WDT_OFF);
	printf("Veamos el valor de MCLR_EN: %X\r\n",MCLR_EN);
	printf("Veamos el valor de PWRT_OFF: %X\r\n",PWRT_OFF);
	printf("Veamos el valor de CODE_PROT_OFF: %X\r\n",CODE_PROT_OFF);

//-----Registros asociados al clock


// Analisis de la UART
	printf("UART_EN: %X\r\n",UART_EN);// asi puedo ver todos los bits del registro que "pisa" UART_EN
	printf("UART_DIS: %X\r\n",UART_DIS);// asi puedo ver todos los bits del registro que "pisa" UART_DIS
	printf("UART_1STOPBIT: %X\r\n",UART_1STOPBIT);// asi puedo ver todos los bits del registro que "pisa" UART_1STOPBIT

	printf("\nU1MODEvalue: %X\r\n",U1MODEvalue);// asi puedo ver todos los bits del registro que "pisa" U1MODEvalue
	printf("U1STAvalue: %X\r\n",U1STAvalue);// asi puedo ver todos los bits del registro que "pisa" U1STAvalue
	printf("BRGvalue: %d\r\n\n",BRGvalue);// asi puedo ver todos los bits del registro que "pisa" baud

	baudrate= 30000000.0 /( 16*(float)(BRGvalue+1)); // pag 496/760 del dsPIC30F Familly Reference Manual
	printf("Baud Rate= %f\r\n",baudrate);

	ERROR=((baudrate-56000.0)/56000.0)*100.0;
	printf("ERROR= %1.1f",ERROR);
	printf(" %%\r\n");
	printf("(Ver pag. 496/760 del dsPIC30F Familly Reference Manual)\r\n");



	stop = clock();
	count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY: %.1f\r\n\n",count);
	Tiempoej=count/30000000.0;
	printf("Tiempo ejecucion=%2.7f",Tiempoej);





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










