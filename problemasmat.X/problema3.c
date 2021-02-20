/*
 * File:   prob.c
 * Author: root
 *
 * Created on May 19, 2020, 11:31 PM
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

#define DIV 4
double Af[3][3]={{0.02/DIV,0.12/DIV,-0.12/DIV},{0.13/DIV,-0.15/DIV,1.2/DIV},{0.28/DIV,-0.28/DIV,0.1/DIV}};
double Bf[3][3]={{1.3/DIV,0.13/DIV,-0.15/DIV},{0.21/DIV,0.18/DIV,-0.01/DIV},{-0.03/DIV,0.17/DIV,0.13/DIV}};
float Aff[3][3]={{0.02,0.12,-0.12},{0.13,-0.15,1.2},{0.28,-0.28,0.1}};
float Bff[3][3]={{1.3,0.13,-0.15},{0.21,0.18,-0.01},{-0.03,0.17,0.13}};
double ABf[3][3];
double BAf[3][3];
float If[3][3];
float AINV[3][3];
float AINVF[3][3];
float BINV[3][3];
float BINVF[3][3];

fractional A[3][3]={{Q15(0.02/DIV),Q15(0.12/DIV),Q15(-0.12/DIV)},{Q15(0.13/DIV),Q15(-0.15/DIV),Q15(1.2/DIV)},{Q15(0.28/DIV),Q15(-0.28/DIV),Q15(0.1/DIV)}};
fractional B[3][3]={{Q15(1.3/DIV),Q15(0.13/DIV),Q15(-0.15/DIV)},{Q15(0.21/DIV),Q15(0.18/DIV),Q15(-0.01/DIV)},{Q15(-0.03/DIV),Q15(0.17/DIV),Q15(0.13/DIV)}};
fractional AB[3][3];
fractional BA[3][3];
clock_t start, stop;
static double count=0;

void imprimir(fractional mat[3][3]);
void multimat(fractional ab[3][3],fractional a[3][3],fractional b[3][3],double af[3][3],double bf[3][3],double abf[3][3]);
void inversaf(float m[3][3]);
void inversa(float mat[3][3],float matinv[3][3]);
void comprobar(void);

int main(void) 
{
    printf("\t\t A*B\n\n");
    multimat(AB,A,B,Af,Bf,ABf);
    printf("\t\t B*A\n\n");
    multimat(BA,B,A,Bf,Af,BAf);
    printf("\t\t A ^ -1\n\n");
    inversa(Aff,AINV);
    inversaf(Aff);
    printf("\t\t B ^ -1\n\n");
    inversa(Bff,BINV);
    inversaf(Bff); 
    printf("\t\t|||| A*(A^-1)=I ||||\n\n");
    comprobar();
    return 0;
}

void multimat(fractional ab[3][3],fractional a[3][3],fractional b[3][3],double af[3][3],double bf[3][3],double abf[3][3])
{
    int i=0;
    int j=0;
    start=clock();
    MatrixMultiply(3,3,3,ab[0],a[0],b[0]);
    stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con dsp: %.1f\r\n",count);
    printf("# TCY: 36+numRows1*(8+numCols2*(7+4*numCols1Rows2)), %d\n\n",36+3*(8+3*(7+4*3)));
    imprimir(ab);
    start=clock();
    while (i<3){
        while (j<3){
            abf[i][j]=af[i][0]*bf[0][j]+af[i][1]*bf[1][j]+af[i][2]*bf[2][j];
            j++;
        }
        j=0;
        i++;
    }
    stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock  
    printf("# TCY con float: %.1f\r\n",count);
    printf("\t\t\t CON FLOAT \n\n");
    i=0;
    j=0;
    while (i<3){
        while (j<3){
            printf("%10f[%d][%d]",abf[i][j]*DIV*DIV,i,j);
            j++;
        }
        j=0;
        printf("\n");
        i++;
    }
    printf("\r\n\n");
}

void inversa(float mat[3][3],float matinv[3][3])
{
    int i=0,j=0;
    int swappedrow[3];
    int swappedcol[3];
    float pivot[3];
    start=clock();
    MatrixInvert(3,matinv[0],mat[0],pivot,swappedrow,swappedcol);
    stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con dsp: %.1f\r\n\n",count);
    while (i<3){
        while (j<3){
            printf("%10f[%d][%d]",(double)matinv[i][j],i,j);
            j++;
        }
        j=0;
        printf("\n");
        i++;
    }
    printf("\r\n\n");
    
}

void inversaf(float m[3][3]){
    float det=0;
    start=clock();
    int i=0;
    int j=0;
    while (i<3){
      det = det + (m[0][i]*(m[1][(i+1)%3]*m[2][(i+2)%3] - m[1][(i+2)%3]*m[2][(i+1)%3]));
      i++;
    }
    i=0;
    while(i<3){
        while (j<3){
           AINVF[i][j]=(((m[(j+1)%3][(i+1)%3] * m[(j+2)%3][(i+2)%3]) - (m[(j+1)%3][(i+2)%3]*m[(j+2)%3][(i+1)%3]))/ det);
           j++;
        }
        j=0;
        i++;
    }
    stop=clock();
    count=(stop-start)-19.0;  // 19 TCY start clock
	printf("# TCY con float: %.1f\r\n\n",count);
    i=0;
    j=0;
    while (i<3){
        while (j<3){
            printf("%10f[%d][%d]",(double)AINVF[i][j],i,j);
            j++;
        }
        j=0;
        i++;
        printf("\n");
        
	}
    printf("\r\n\n");
}

void imprimir(fractional mat[3][3])
{
    printf("\t\t\t CON DSP \n\n");
    int i=0,j=0;
    while (i<3){
        while (j<3){
            printf("%10f[%d][%d]",(double)Fract2Float(mat[i][j])*DIV*DIV,i,j);
            j++;
        }
        j=0;
        printf("\n");
        i++;
    }
    printf("\n\n\r");
}

void comprobar(void){
    int i=0;
    int j=0;
    while (i<3){
        while (j<3){
            If[i][j]=Aff[i][0]*AINV[0][j]+Aff[i][1]*AINV[1][j]+Aff[i][2]*AINV[2][j];
            j++;
        }
        j=0;
        i++;
    }
    i=0;
    j=0;
    while (i<3){
        while (j<3){
            printf("%10f[%d][%d]",(double)If[i][j],i,j);
            j++;
        }
        j=0;
        printf("\n");
        i++;
    }
    printf("\r\n\n");
    
}