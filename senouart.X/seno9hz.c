/*
 * File:   seno9hz.c
 * Author: andres 2
 *
 * Created on 20 de mayo de 2020, 08:58 PM
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
double coeff[5]={1.9037,-1.0,0.0004258,0.0008517,0.0004258};
double valf[5] ={(0.0),(0.0),(1.0),(0.0),(0.0)};
double Yf[1];
fractional coef[5]={Q15(1.9037/DIV),Q15(-1.0/DIV),Q15(0.0004258/DIV),Q15(0.0008517/DIV),Q15(0.0004258/DIV)};
fractional val[5] ={Q15(0.0),Q15(0.0),Q15(1),Q15(0.0),Q15(0.0)};
fractional Y[1];
clock_t start, stop;
static double count=0;


int main(void) 
{
    printf("\t\t\t\t CON DSP \n\n\n\n");
    //start=clock();
    int i=0;
    while (i<20){
        start=clock();
        Y[0]=VectorDotProduct(5,val,coef);
        Y[0]=Y[0]<<1;
        val[1]=val[0];
        val[4]=val[3];
        val[3]=val[2];
        val[2]=0;
        val[0]=Y[0];
        stop=clock();
        //printf("%f\n",(double)Fract2Float(Y[0]));
        i++;
    }
    //stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("\n\n# TCY con DSP: %.1f\r\n",count);
    int j=0;
    i=0;
    double Yf=0.0;
    printf("\t\t\t\t CON FLOAT \n\n\n\n");
    //start=clock();
    while (j<20){
        start=clock();
        while (i<5)
        {
            Yf=Yf+coeff[i]*valf[i];
            i++;
        }
        i=0;
        Yf=Yf*2;
        //printf("%f\n",Yf);
        valf[1]=valf[0];
        valf[4]=valf[3];
        valf[3]=valf[2];
        valf[2]=0;
        valf[0]=Yf;
        stop=clock();
        Yf=0;
        j++;
    }
    //stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con float: %.1f\r\n",count);
    return 0;
}