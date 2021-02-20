/*
 * File:   problemas.c
 * Author: root
 *
 * Created on May 8, 2020, 4:14 PM
 */


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

#define DIV 4.4

double b[3]={0.21/DIV,-0.3/DIV,1.2/DIV};
double c[3]={0.2/DIV,2.2/DIV,1.3/DIV};
double vec1[15]={0.001,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01};
double vec2[15]={0.03,0.02,0.05,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01,0.01};
double destf[15];
double prodf[3];
double addf[3];
double subf[3];
double *pc=c;
double *pb=b;

fractional d[3]={Q15(0.21/DIV),Q15(-0.3/DIV),Q15(1.2/DIV)};
fractional e[3]={Q15(0.2/DIV),Q15(2.2/DIV),Q15(1.3/DIV)};
fractional vec1_15[15]={Q15(0.001),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01)};
fractional vec2_15[15]={Q15(0.03),Q15(0.02),Q15(0.05),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01),Q15(0.01)};
fractional dest[15];
fractional prod[3];
fractional add[3];
fractional sub[3];
fractional *pd;
fractional *pe;

clock_t start, stop;
static double count=0;

float maximo(float** vctor1,float** vctor2, int large);
void normalizar(float** vect1,float** vect2,fractional** vctor1,fractional** vctor2, int large,float maxi);
void vecmulti(void);
void vecmulti15(void);
void vectoradd(void);
void vectorsub(void);
void vectorpow(void);
void vectordot(void);
void imprimir(fractional a[],int large);

int main(void) 
{
    vecmulti();
    vectoradd();
    vectorsub();
    vectorpow();
    vectordot();
    vecmulti15();
    return 0;
}

void vecmulti(void)
{
    printf("\t\t\t\t\t |||||MULTIPLICACION||||| \t\t\n\n");
    start=clock();
    VectorMultiply(3,prod,d,e);
    stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con dsp: %.1f\r\n",count);
    printf("# TCY: En realidad deberia dar: [17 + 4(numElems)]= 17+4*3=29 !\r\n\n");
    imprimir(prod,3);
    int i=0;
    start=clock();
    while (i<3)
    {
        prodf[i]=b[i]*c[i];
        i++;
    }
    stop=clock();
    i=0;
    while (i<3)
    {
        printf("Valor float[%d]= %.7f \n\r",i, prodf[i]*DIV*DIV);
        i++;
    }
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con float: %.1f\r\n",count);
    
}

void vecmulti15(void)
{
    printf("\t\t\t\t\t |||||MULTIPLICACION VECTOR DE 15||||| \t\t\n\n");
    start=clock();
    VectorMultiply(15,dest,vec1_15,vec2_15);
    stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con dsp: %.1f\r\n",count);
    printf("# TCY: En realidad deberia dar: [17 + 4(numElems)]= 17+4*15=77 !\r\n\n");
    int j=0;
    while (j<15){
		printf("Valor dsp[%d]= %.15f \n\r",j,(double)Fract2Float(dest[j]));
        j++;
    }
    j=0;
    start=clock();
    while (j<15)
    {
        prodf[j]=vec1[j]*vec2[j];
        j++;
    }
    stop=clock();
    j=0;
    while (j<15)
    {
        printf("Valor float[%d]= %.7f \n\r",j, prodf[j]);
        j++;
    }
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con float: %.1f\r\n",count);
}

void vectoradd(void)
{
    printf("\t\t\t\t\t |||||SUMA||||| \t\t\n\n");
    start=clock();
    VectorAdd(3,add,d,d);
    stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con dsp: %.1f\r\n",count);
    printf("# TCY: En realidad deberia dar: [17 + 3(numElems)]= 17+3*3=26 !\r\n\n");
    int j=0;
    while (j<3){
		printf("Valor dsp[%d]= %.7f \n\r",j,(double)Fract2Float(add[j])*DIV);
        j++;
    }
    start=clock();
    j=0;
    while (j<3)
    {
        addf[j]=b[j]+b[j];
        j++;
    }
    stop=clock();
    j=0;
    while (j<3)
    {
        printf("Valor float[%d]= %.7f \n\r",j, addf[j]*DIV);
        j++;
    }
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con float: %.1f\r\n",count);

}
void vectorsub(void)
{
    printf("\t\t\t\t\t |||||RESTA||||| \t\t\n\n");
    start=clock();
    VectorSubtract(3,sub,d,e);
    stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con dsp: %.1f\r\n",count);
    printf("# TCY: En realidad deberia dar: [17 + 4(numElems)]= 17+4*3=29 !\r\n\n");
    int j=0;
    while (j<3){
		printf("Valor dsp[%d]= %.7f \n\r",j,(double)Fract2Float(sub[j])*DIV);
        j++;
    }
    start=clock();
    j=0;
    while (j<3)
    {
        subf[j]=b[j]-c[j];
        j++;
    }
    stop=clock();
    j=0;
    while (j<3)
    {
        printf("Valor float[%d]= %.7f \n\r",j, subf[j]*DIV);
        j++;
    }
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con float: %.1f\r\n",count);
}

void vectorpow(void)
{
    printf("\t\t\t\t\t |||||POTENCIA||||| \t\t\n\n");
    fractional power;
    start=clock();
    power=VectorPower(3,e);
    stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con dsp: %.1f\r\n",count);
    printf("# TCY: En realidad deberia dar: [16 + 2(numElems)]= 16+2*3=22 !\r\n\n");
    printf("Valor dsp= %.7f \n\r", (double)Fract2Float(power)*DIV*DIV);
    double po;
    int i=0;
    po=0.0;
    start=clock();
    while (i<3)
    {
        po=po+c[i]*c[i];
        i++;
    }
    stop=clock();
    i=0;
    printf("Valor float= %.7f \n\r", po*DIV*DIV);
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con float: %.1f\r\n",count);
}

void vectordot(void)
{
    printf("\t\t\t\t\t |||||PRODUCTO ESCALAR||||| \t\t\n\n");
    fractional dot;
    start=clock();
    dot=VectorDotProduct(3,d,e);
    stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con dsp: %.1f\r\n",count);
    printf("# TCY: En realidad deberia dar: [17 + 3(numElems)]= 17+3*3=26 !\r\n\n");
    printf("Valor dsp= %.7f \n\r", (double)Fract2Float(dot)*DIV*DIV);
    double dotf;
    int i=0;
    dotf=0.0;
    start=clock();
    while (i<3)
    {
        dotf=dotf+b[i]*c[i];
        i++;
    }
    stop=clock();
    i=0;
    printf("Valor float= %.7f \n\r", dotf*DIV*DIV);
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con float: %.1f\r\n",count);
}



void imprimir(fractional a[],int large)
{
    int j=0;
    while (j<large){
		printf("Valor dsp[%d]= %.7f \n\r",j,(double)Fract2Float(a[j])*DIV*DIV);
        j++;
    }
}

float maximo(float** vctor1,float** vctor2, int large)
{
    float max=(*(*vctor1));
    int i=0;
    while (i<large)
    {
        if (*(*vctor1+i)>max)
        {
            max=(*(*vctor1+i));
        }
        i++;
    }
    i=0;
    while (i<large)
    {
        if (*(*vctor2+i)>max)
        {
            max=(*(*vctor2+i));
        }
        i++;
    }    
    return max;
}

void normalizar(float** vect1,float** vect2,fractional** vctor1,fractional** vctor2, int large,float ma)
{
    int i=0;
    while (i<large)
    {
        printf("%f %f\n\n",(*(*(vect1)+i))/ma,*(*(vect2)+i)/ma);
        *(*(vctor1)+i)=Q15(((*(*(vect1)+i))/ma));
        *(*(vctor2)+i)=Q15(((*(*(vect2)+i))/ma));
        i++;
    }
}
