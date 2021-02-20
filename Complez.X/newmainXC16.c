/*
 * File:   newmainXC16.c
 * Author: root
 *
 * Created on July 6, 2020, 5:30 PM
 */


#include "xc.h"
#include <stdio.h>
#include <p33FJ256GP510.h>

int main(void) {
    _Complex int z1=5.0j;
    _Complex int z2=5.0j;
    _Complex int z3=0;
    z3=z1*z2;
    printf("%d",z2);
    return 0;
}
