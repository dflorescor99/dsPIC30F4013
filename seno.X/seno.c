/*
 * File:   seno.c
 * Author: root
 *
 * Created on May 17, 2020, 3:11 PM
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

#define DIV 1

double b[3]={0.21/DIV,-0.3/DIV,1.2/DIV};
double c[3]={0.2/DIV,2.2/DIV,1.3/DIV};


fractional coef[5]={Q15(0.9921/DIV),Q15(-0.5/DIV),Q15(0.0313/DIV),Q15(0.0626/DIV),Q15(0.0313/DIV)};
fractional val[5] ={Q15(0.0),Q15(0.0),Q15(0.5),Q15(0.0),Q15(0.0)};
fractional sum[5];
fractional Y[1];
fractional dosY[1];
clock_t start, stop;
static double count=0;


int main(void) 
{
    
    int i=0;
    while (i<101){
        Y[0]=VectorDotProduct(5,val,coef);
        VectorAdd(1,dosY,Y,Y);
        val[1]=val[0];
        val[4]=val[3];
        val[3]=val[2];
        val[2]=0;
        val[0]=dosY[0];
        printf("%f\n",(double)Fract2Float(dosY[0]));
        i++;
    }
    return 0;
}
