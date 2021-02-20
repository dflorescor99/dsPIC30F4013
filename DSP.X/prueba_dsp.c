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

//	float x[4];
//	float minimo, maximo;
	clock_t start, stop;
 	unsigned int j;
	double x[4],y;

	fractional srcV1[4]={Q15(0.05), Q15(-0.0456), Q15(-0.4854), Q15(0.5)};
	fractional srcV2[4]={Q15(0.05), Q15(0.1), Q15(-0.1), Q15(-0.5)};
	fractional dstV[4];
	fractional *psrcV1;
	fractional *psrcV2;
	fractional *pdstV;

	fractional src1=Q15(0.5);
	fractional src2=Q15(0.4);
	fractional dst;

	double src1_float,src2_float,dst_float;
	static double count=0;
//	int n=4;
//	int min;
//	int max;


void producto_vectores (void);
void producto_numeros (void);

int main (void)

	{


// printf formateadores: https://es.cppreference.com/w/cpp/io/c/printf_format

	printf("\n\n\t\t\t ///Hola Conrado, DSP en Dspic30f4013///\r\n\n");
	


producto_vectores();  // usando DSP
producto_numeros();	// sin usar DSP y usando DSP
	

while(1);
}


void producto_vectores (void)
{
	printf("Prueba de producto de vectores\r\n");
	start = clock();   // Marca #Tcy

	VectorMultiply (4, &dstV[0],&srcV1[0],&srcV2[0]);  // library function del DSP

	stop = clock(); // toc

	//O bien: 

	pdstV=&dstV[0];
	psrcV1=&srcV1[0];
	psrcV2=&srcV2[0];
	VectorMultiply (4, pdstV,psrcV1,psrcV2);  // library function del DSP
	

	 //O bien:

	VectorMultiply (4, dstV,srcV1,srcV2);  // library function del DSP


	// si uno define un vector Vector[4]
	// Vector solito (sin []) es la direccion de memoria a Vector [0]
	// y si definis un puntero: fractional *pvector ...
	// luego hay que inicializarlo con pvector=&(Vector[0])
	
	
	count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY: %.1f\r\n",count);
	printf("# TCY: En realidad deberia dar: [17 + 4(numElems)]= 17+4*4=33 !\r\n\n");


	for (j=0;j<=3;j++)
		{
		x[j]=Fract2Float(dstV[j]);	
		printf("Valor Producto x[%u]= %.7f \n\r",j,x[j]);
		}
		}

void producto_numeros (void)
{
//-------------------------------------------------------

	printf("Prueba de producto de escalares con el modulo DSP\r\n");

	start = clock();   // tic: Marca #Tcy
		VectorMultiply (1, &dst, &src1, &src2);		// usando DSP
	stop = clock();
	count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY: %.1f\r\n",count);
	printf("# TCY: En realidad deberia dar: [17 + 4(numElems)]= 17+4*1=21 !\r\n\n");
	y=Fract2Float(dst);	
	printf("Valor Producto escalares usando DSP= %.7f \n\r",y);

	src1_float=	Fract2Float(src1);
	src2_float=	Fract2Float(src2);

	start = clock();   // tic: Marca #Tcy
		dst_float=src1_float*src2_float;// sin usar DSP
				asm("nop");	/// compilar assembler equivale a Nop();
				asm("nop");							
	stop = clock();
	count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY: %.1f\r\n",count);
	//printf("# TCY: En realidad deberia dar: [17 + 4(numElems)]= 17+4*1=21 !\r\n\n");
	y=dst_float;	
	printf("Valor Producto escalares sin usar DSP= %.7f \n\r",y);
}

//-------------------------------------------------------

//	TERM_reset;	// Hyperterminal like VT100
//	TERM_BACK_black;
//	TERM_FORE_green;
//	TERM_FORE_bright;
//	TERM_clear;
//	TERM_HOME;



	
//	printf("\n\t\t\t ///Prueba modulo DSP///\r\n\n");
//	Nop();
//
//	for (j=0;j<=3;j++)
//		{
//		x[j]=Fract2Float(dstV[j]);		//demora 6061 ciclos maso 151 useg a 40 MIPS
//		printf("Valor Vector x[%u]= %.5f \n\r",j,x[j]);
//		}
//	printf("\r\n");
//
//
//	start = clock();	// Copia de vectores
//	VectorCopy (n, dstV, srcV);
//	stop = clock();
//	count=(stop-start)-19.0;  // 19 TCY start clock
//	printf("TCY VectorCopy: %.1f\r\n\n",count);
//
//
//
//	start = clock();	// Instruccion printf
//	printf("hola mundo\n\r");
//	stop = clock();
//	count=(stop-start)-19.0;  // 19 TCY start clock
//	printf("TCY printf: %.1f\r\n\n",count);
//
//
//
//	start = clock();	// prueba de VectorMin
//	VectorMin(4, srcV, &min); // 37 TCY
//	stop = clock();
//	count=(stop-start)-19.0;  // 19 TCY start clock
//	printf("TCY de VectorMin: %.1f\r\n",count);
//	printf("Posicion del minimo: %i\r\n",min);
//	minimo=Fract2Float(srcV[min]);
//	printf("Valor del minimo: x[%u]= %.5f\r\n",min,minimo);
//	printf("\r\n");
//	printf("\r\n");
//
//
//	start = clock();	// prueba de VectorMax
//	VectorMax(4, srcV, &max); // 35/36.. TCY
//	stop = clock();
//	count=(stop-start)-19.0;  // 19 TCY start clock
//	printf("TCY de VectorMax: %.1f\r\n",count);
//	printf("Posicion del maximo: %i\r\n",max);
//	maximo=Fract2Float(srcV[max]);
//	printf("Valor del maximo: x[%u]= %.5f\r\n",max,maximo);
//	printf("\r\n");
//	printf("\r\n");
//	
//	
//	
//
//	// printf formateadores: https://es.cppreference.com/w/cpp/io/c/printf_format
//
//	printf("\n\n\t\t\t ///Hola Conrado, DSP en Dspic30f4013///\r\n\n");
//	Nop();
//
//
//
//
//
//
//
//while(1);
//
//
//
////-------------------------------------------------------
//
//	printf("Prueba de funcion Fract2Float\r\n");
//
//// Horror en control !!
//
//start = clock();   // Marca #Tcy
//	for (j=0;j<=3;j++)
//		{
//		x[j]=Fract2Float(dstV[j]);	
//		//printf("Valor Producto x[%u]= %.5f \n\r",j,x[j]);
//		}
//
//	stop = clock();
//	count=(stop-start)-19.0;  // 19 TCY start clock
//	printf("# TCY: %.1f\r\n\n",count);
//
////-------------------------------------------------------
//
//	while(1);
//
////	printf("h");
//
//	while(1)
//	{
//	printf("aca esta el marker !\r\n");
//	}
//	printf("\r\nstart = %ld", start);
//	printf("\r\nstop = %ld", stop);
//
//
//	
//
//	while(1);	// pie final aca te quedas...
//	}
//
//
//
//
//
//
//



