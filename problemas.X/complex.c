/*
 * File:   complex.c
 * Author: root
 *
 * Created on August 26, 2020, 9:50 PM
 */


#include <stdio.h>
#include <p30F4013.h>// si se cambia de DSC cambiar tb p30FXXXX.gld linker
#include <libpic30.h>
#include <stdio.h>
#include <string.h>
#include <dsp.h>
#include <time.h>           // tic toc
#include <uart.h>

#define COMPLEX __complex__

int main(argc, argv)
int argc;
char *argv[];
{
    COMPLEX double z1, z2, z3, z4;
    double creal(), cimag();
    
    
    z1 = 1 + 1i;
    z2 = 1 - 1i;
    z3 = z1*z2;
    z4 = z3/z1;

    printf("%g + %gi\n", creal(z4), cimag(z4));
      
    return 0;
}