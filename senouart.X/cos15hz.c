/*
 * File:   cos15hz.c
 * Author: root
 *
 * Created on May 21, 2020, 10:34 AM
 */

#include "xc.h"
#define MIPS 30
#include <p30F4013.h>		// si se cambia de DSC cambiar tb p30FXXXX.gld linker
#include <stdio.h>
#include <string.h>
#include <dsp.h>
#include <time.h>           // tic toc
#include <uart.h>


_FOSC(CSW_FSCM_OFF & XT_PLL16);  
_FWDT(WDT_OFF);                 
_FBORPOR(MCLR_EN & PWRT_OFF);  
_FGS(CODE_PROT_OFF);     

char a;			/* Variable 'a' de tipo char */
char *pchar; 	/* Puntero a char 'pchar' */

#define DIV 2

fractional coef[5]={Q15(1.9843/DIV),Q15(-1.0/DIV),Q15(0.0006640/DIV),Q15(0.0000695/DIV),Q15(-0.000664/DIV)};
fractional val[5] ={Q15(0.0),Q15(0.0),Q15(1),Q15(0.0),Q15(0.0)};
fractional sum[5];
fractional Y[1];
clock_t start, stop;
static double count=0;


int main(void) 
{
    
    int i=0;
    while (i<201){
        Y[0]=VectorDotProduct(5,val,coef);
        Y[0]=Y[0]<<1;
        val[1]=val[0];
        val[4]=val[3];
        val[3]=val[2];
        val[2]=0;
        val[0]=Y[0];
        printf("%f\n",(double)Fract2Float(Y[0]));
        i++;
    }
    return 0;
}