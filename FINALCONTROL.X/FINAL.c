/*
 * File:   FINAL.c
 * Author: root
 *
 * Created on February 20, 2021, 3:57 AM
 */

#include "xc.h"
#include <p30F4013.h>
#include <stdio.h>
#define MIPS 30
#define B9 LATBbits.LATB9
#define B10 LATBbits.LATB10
#define B11 LATBbits.LATB11
#define B12 LATBbits.LATB12

_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

void configuarttx();
void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void writeshex(uint16_t A, uint16_t B);
void CN(void);
void ADC(void);
void pwm_conf(void);
void timer_conf(void);

uint16_t AD0=0;
uint16_t AD1=0;
//float ADCf = 0.0;
//int ADCint = 0;

int main(void) 
{
    //TRISBbits.TRISB11 = 0;
    //TRISBbits.TRISB4 = 0;
    //pwm_conf();
    uint16_t AD0=0;
    uint16_t AD1=0;
    //CN();
    ADC();
    configuarttx();
    timer_conf();
    IEC0bits.U1RXIE = 0;
    IEC0bits.U1TXIE = 0;
    //IEC0bits.ADIE=1;
    //IFS0bits.ADIF=0;
    writeshex(AD0,AD1);
    ADCON1bits.ADON = 1;
    while (1) // repeat continuously

    {
        while (!IFS0bits.ADIF);
        AD0 = ADCBUF0;
        AD1 = ADCBUF1;
        writeshex(AD0,AD1);
    }
    return 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _ADCInterrupt(void)
{
    ADCON1bits.ADON = 0;   // this must be executed before the end of 17th conversion
    IFS0bits.ADIF = 0;
    AD0 = ADCBUF0;
    AD1 = ADCBUF1;
    ADCON1bits.ADON = 1;
}

void ADC()
{
    ADPCFG = 0b1111111111101011; //Ponemos a RB2 Y RB4 como entradas analogicas
    ADCON1 = 0b0000000001000100; //El bit ASAMP esta seteado para que el muestreo empiece automaticamente despues de la conversion, 
                                 //SSRC esta en 010 para que el timer 3 se encargue de la duracion del sampleo
    ADCON2 = 0b0000010000000100; //Seteamos el bit CSCNA para escanear multiples entradas, eso hace que la seleccion de canales en 
                                 //ADCHS sea ignorada,SMPI esta en 0001 para que la interrupcion se de despues de completar la segunda secuencia de muestreo/conversion  
    ADCON3 = 0b0000000000000010; //Con esta configuracion un TAD = TCY/2
    ADCHS =  0b0000000000000010;
    ADCSSL = 0b0000000000010100; // Escaneo del canal, elegimos AN2 Y AN4 como fuentes para el canal
}

void CN(void)
{
    CNEN1bits.CN5IE = 1; // Enable CN3 pin for interrupt detection
    CNPU1bits.CN5PUE=1;
    IEC0bits.CNIE = 1; // Enable CN interrupts
    IFS0bits.CNIF = 0; // Reset CN interrupt
}

void pwm_conf(void)
{
    TRISDbits.TRISD0=0;
    PR2=3000; //PERIODO DEL PWM (Tpwm/(TCY*PRESCALER))-1 -> Tpwm= [(PR2) + 1]*TCY*Prescaler, queremos una frecurncia de 10KHz
    TMR2=0; // DONDE ARRANCA EL TIMER 2
    OC1R=0; //TON PARA EL PRIMER CICLO
    OC1RS=0;
    OC1CONbits.OCM=0b110;
    OC1CONbits.OCTSEL=0;
    T2CONbits.T32=0; //SI QUEREMOS UN TIMER DE 32 O 16 BITS
    T2CONbits.TCKPS=0b00; //ELEGIMOS EL PRESCALER
    T2CONbits.TCS=0; //ELEGIMOS QUE QUEREMOS SEGUIR LAS INSTRUCCIONES INTERNAS COMO ORIGEN DEL TIMER
    T2CONbits.TON=1;//ENCENDEMOS EL TIMER2
}

void timer_conf(void)
{
    TMR3 = 0;
    PR3 = 9374;           //Periodo=(PR3+1)*TCY*PREESCALER
    T3CONbits.TCKPS=0b10; //ELEGIMOS EL PRESCALER
    T3CONbits.TCS=0;      //ELEGIMOS QUE QUEREMOS SEGUIR LAS INSTRUCCIONES INTERNAS COMO ORIGEN DEL TIMER
    T3CONbits.TON=1;      //ENCENDEMOS EL TIMER3
}
/*
void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    IFS0bits.CNIF = 0;
    CNEN1bits.CN5IE = 0;
    if (~LATBbits.LATB2){
        delay_ms(200);
        B11=~B11;
        //delay_ms(200);
    }
    //delay_ms(200);
    CNEN1bits.CN5IE = 1;
}
 */

void configuarttx(void)
{
    OSCTUN=0; 
    RCONbits.SWDTEN=0;
    U1MODEbits.UARTEN = 0;// Disable UART
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Pari vcxty, 8-data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1BRG = 15;           // UxBRG = Fcy/(16*BaudRate) - 1, elegimos un Baudrate de 115200
    U1STAbits.UTXISEL = 0;         // Interrupcion cuando mandamos un caracter
    U1STAbits.URXISEL = 0;         // Interrupcion cuando recibimos un caracter
    U1MODEbits.UARTEN = 1;         // Enable UART
    U1STAbits.UTXEN = 1;           // Enable UART Tx, solo activamos el envio
}

void writeshex(uint16_t A, uint16_t B)
{
    uint16_t mask=0x00FF;
    uint8_t FRAMEH=0x03;
    uint8_t FRAMEL=0xFC;
    uint8_t ADCLOW=mask&A;
    uint8_t ADCHIGH=mask&(A>>8);
    uint8_t ADCLOW1=mask&B;
    uint8_t ADCHIGH1=mask&(B>>8);
    
    U1TXREG=FRAMEH;
    delay_us(50);
    U1TXREG=ADCLOW;
    delay_us(50);
    U1TXREG=ADCHIGH;
    delay_us(50);
    U1TXREG=ADCLOW1;
    delay_us(50);
    U1TXREG=ADCHIGH1;
    delay_us(50);
    U1TXREG=FRAMEL;
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