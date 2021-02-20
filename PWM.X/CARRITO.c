/*
 * File:   CARRITO.c
 * Author: root
 *
 * Created on June 30, 2020, 1:43 AM
 */

void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void configuart(void);
void pwm_conf(void);
void recivechar(void);

#include "xc.h"
#include <p30F4013.h>
#define MIPS 30

_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

static char rx;

int main(void) 
{
    int aux=0;
    pwm_conf();
    configuart();
    OC1RS=20000; //IZQUIERDA
    OC2RS=20000; //DERECHA
    OC3RS=0000;
    OC4RS=0000;
    int i=0;
    while (1){
        while(!IFS0bits.U1RXIF);
        recivechar();
        U1TXREG=rx;
        if (rx=='w'){
            OC3RS=0;
            OC4RS=0;
            if (OC1RS==0){
                i=1;
                OC1RS=6000;
                OC2RS=OC1RS+1000;
            }
            if ((OC2RS <= 21100) && (i==0)){
                OC1RS=OC1RS+300;
                OC2RS=OC1RS+1000;
            }
            i=0;
        }
        
        else if (rx=='a'){
            aux=OC1RS;
            OC1RS=0;
            OC2RS=10000;
            delay_ms(100);
            OC1RS=aux;
            OC2RS=aux;
        }
        
        else if (rx=='d'){
            aux=OC2RS;
            OC2RS=0;
            OC1RS=10000;
            delay_ms(100);
            OC1RS=aux;
            OC2RS=aux;
        }
        
        else if (rx=='s'){
            OC1RS=0;
            OC2RS=0;
            delay_ms(100);
            if ((OC3RS <= 6000) && (OC4RS <= 6000)){
                OC4RS=8000;
                OC3RS=8000;
            }
            
        }
    }
}

void pwm_conf(void)
{
    //DEFINIMOS LOS PUERTOS DEL PWM COMO SALIDA DIGITAL
    
    TRISDbits.TRISD0=0; //OC1
    TRISDbits.TRISD1=0; //0C2
    TRISDbits.TRISD2=0; //0C3
    TRISDbits.TRISD3=0; //0C4
    
    //VAMOS A MANEJAR TODO CON EL MISMO TIMER, EL TIMER 2
    
    PR2=21591; //PERIODO DEL PWM (Tpwm/(TCY*PRESCALER))-1
    TMR2=0; // DONDE ARRANCA EL TIMER 2
    
    //PWM 1
    OC1R=0; //TON PARA EL PRIMER CICLO
    OC1RS=0;
    OC1CONbits.OCM=0b110;
    OC1CONbits.OCTSEL=0;
    
    //PWM 2
    OC2R=0; //TON PARA EL PRIMER CICLO
    OC2RS=0;
    OC2CONbits.OCM=0b110;
    OC2CONbits.OCTSEL=0;
    
    //PWM 2
    OC3R=0; //TON PARA EL PRIMER CICLO
    OC3RS=0;
    OC3CONbits.OCM=0b110;
    OC3CONbits.OCTSEL=0;
    
    //PWM 4
    OC4R=0; //TON PARA EL PRIMER CICLO
    OC4RS=0;
    OC4CONbits.OCM=0b110;
    OC4CONbits.OCTSEL=0;
    
    
    T2CONbits.T32=0; //SI QUEREMOS UN TIMER DE 32 O 16 BITS
    T2CONbits.TCKPS=0b00; //ELEGIMOS EL PRESCALER
    T2CONbits.TCS=0; //ELEGIMOS QUE QUEREMOS SEGUIR LAS INSTRUCCIONES INTERNAS COMO ORIGEN DEL TIMER
    T2CONbits.TON=1;//ENCENDEMOS EL TIMER2
}

void configuart(void)
{
    OSCTUN=0; 
    RCONbits.SWDTEN=0;
    U1MODEbits.UARTEN = 0;// Disable UART
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1BRG = 194;          //9600
    U1STAbits.UTXISEL = 0;         // Interrupcion cuando mandamos un caracter
    U1STAbits.URXISEL = 0;         // Interrupcion cuando recibimos un caracter
    U1MODEbits.UARTEN = 1;         // Enable UART
    U1STAbits.UTXEN = 1;           
    IFS0bits.U1RXIF=0;
}

void recivechar(void)
{
    IFS0bits.U1RXIF=0;
    rx=U1RXREG;
}

void delay_ms (unsigned long delay_count)

	{
	delay_count=delay_count*MIPS*100;
	while(delay_count--);
	}


void delay_us (unsigned int delay_count)

	{
	delay_count=delay_count*MIPS/4;
	
	while(delay_count--);
	}


