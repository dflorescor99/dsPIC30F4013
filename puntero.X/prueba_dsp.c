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
#include <libq.h>
#include "terminal.h"		// hiperterminal colores
#include "delays.h"



_FOSC(CSW_FSCM_OFF & XT_PLL16);
_FWDT(WDT_OFF);              
_FBORPOR(MCLR_EN & PWRT_OFF);
_FGS(CODE_PROT_OFF);            

////--------------------------------------------

//	float x[4];
//	float minimo, maximo;
	clock_t start, stop;
 	unsigned int j;
	double x[4],y;
//	fractional srcV1[4]={Q15(0.05), Q15(-0.0456), Q15(-0.4854), Q15(0.5)};
//	fractional srcV2[4]={Q15(0.05), Q15(0.1), Q15(-0.1), Q15(-0.5)};
//	fractional dstV[4];

//	fractional src1=Q15(0.5);
//	fractional src2=Q15(0.4);
//	fractional dst;

//	double src1_float,src2_float,dst_float;
//	static double count=0;

//void producto_vectores (void);
//void producto_numeros (void);

// punteros

char a;			/* Variable 'a' de tipo char */
char *pchar; 	/* Puntero a char 'pchar' */


int b[8]={1, 2, 3, 4, 5, 6, 7, 8};

int c[8]={1, 2, 3, 4, 5, 6, 7, 8};
int *pc;

fractional d[4]= {Q15(2), Q15(0.2), Q15(0.3)};
fractional *pd;

int main (void)
{
	printf ("formateadores: https://es.cppreference.com/w/cpp/io/c/printf_format");
	printf("\n\n\t\t\t ///Hola Francisco, manejo de punteros sin DMA con DSP en Dspic30f4013///\r\n\n");
    printf("%f",(float)Fract2Float(d[0]));

	// si uno define un vector Vector[4]
	// Vector solito (sin []) es la direccion de memoria a Vector [0]
	// y si definis un puntero: fractional *pvector ...
	// luego hay que inicializarlo con pvector=&(Vector[0])



	a='h';			// asi cargo la variable con el chr 'h'
	*pchar='h';		// asi tb cargo la variable con el char 'h'
	printf("la direccion de memoria de 'a' es: %p \n", &a);
	pchar = &a;	/* 'pchar' <- @ de 'a' */
	printf("la direccion de memoria de 'a' es: %p \n", &a);
	printf("y su contenido es : %c \n", *pchar);

	
	pc=&(c[0]);;  // haciedo esto se donde el compilador asigno a c
	printf("la direccion de memoria de 'c[0]' es: %p \n",pc);
	printf("y su contenido es c[0]=: %d \n", *pc);
	pc++;
	printf("la direccion de memoria de 'c[1]' es: %p \n",pc);
	printf("y su contenido es c[1]=: %d \n", *pc);
	

	pd=&(d[0]);;  // haciendo esto se donde el compilador asigno a d[0]
	pd=d;		// es lo mismo que hacer pd=&(d[0]), o sea d es equiv a d[0] (primer elemento del vector d[4])

	printf("la direccion de memoria de 'd[0]' es: %p \n",pd);
	printf("y su contenido es d[0]=: %f \n",(double)Fract2Float(*pd));
	pd++;
	printf("la direccion de memoria de 'd[1]' es: %p \n",pd);
	printf("y su contenido es d[1]=: %f \n", (double)Fract2Float(*pd));





	

// inicializacion del puntero
	

	
//producto_vectores();  // usando DSP
//producto_numeros();	// sin usar DSP y usando DSP
	
// prueba punteros






while(1);
}


//void producto_vectores (void)
//{
//	printf("Prueba de producto de vectores\r\n");
//	start = clock();   // Marca #Tcy
//
//	VectorMultiply (4, dstV,srcV1,srcV2);  // library function del DSP
//
//	stop = clock(); // toc
//	count=(stop-start)-19.0;  // 19 TCY start clock
//	printf("# TCY: %.1f\r\n",count);
//	printf("# TCY: En realidad deberia dar: [17 + 4(numElems)]= 17+4*4=33 !\r\n\n");
//
//
//	for (j=0;j<=3;j++)
//		{
//		x[j]=Fract2Float(dstV[j]);	
//		printf("Valor Producto x[%u]= %.7f \n\r",j,x[j]);
//		}
//}
//
//void producto_numeros (void)
//{
////-------------------------------------------------------
//
//	printf("Prueba de producto de escalares con el modulo DSP\r\n");
//
//	start = clock();   // tic: Marca #Tcy
//		VectorMultiply (1, &dst, &src1, &src2);		// usando DSP
//	stop = clock();
//	count=(stop-start)-19.0;  // 19 TCY start clock
//	printf("# TCY: %.1f\r\n",count);
//	printf("# TCY: En realidad deberia dar: [17 + 4(numElems)]= 17+4*1=21 !\r\n\n");
//	y=Fract2Float(dst);	
//	printf("Valor Producto escalares usando DSP= %.7f \n\r",y);
//
//	src1_float=	Fract2Float(src1);
//	src2_float=	Fract2Float(src2);
//
//	start = clock();   // tic: Marca #Tcy
//		dst_float=src1_float*src2_float;// sin usar DSP
//				asm("nop");	/// compilar assembler equivale a Nop();
//				asm("nop");							
//	stop = clock();
//	count=(stop-start)-19.0;  // 19 TCY start clock
//	printf("# TCY: %.1f\r\n",count);
//	//printf("# TCY: En realidad deberia dar: [17 + 4(numElems)]= 17+4*1=21 !\r\n\n");
//	y=dst_float;	
//	printf("Valor Producto escalares sin usar DSP= %.7f \n\r",y);
//}

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



