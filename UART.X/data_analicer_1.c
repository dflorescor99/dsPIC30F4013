/*
 * File:   data_analicer.c
 * Author: root
 *
 * Created on September 9, 2020, 7:34 PM
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

uint16_t AD=0;
uint16_t AD1=0;
float ADCf = 0.0;
int ADCint = 0;

int main(void) 
{
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB4 = 0;
    CN();
    ADC();
    //pwm_conf();
    configuarttx();
    IEC0bits.U1RXIE = 0;
    IEC0bits.U1TXIE = 0;
    IEC0bits.ADIE=1;
    IFS0bits.ADIF=0;
    while (1) // repeat continuously

    {
        ADCON1bits.SAMP = 1;
        delay_us(100); 	// 
        ADCON1bits.SAMP = 0; 		// start Converting
        while (IFS0bits.ADIF==1); 	// Espero que la interrupcion se habilite
        //OC1RS=ADCint;
    }
    return 0;
}


void ADC()
{
    ADPCFG = 0b1111111111100111; 
    ADCON1 = 0x0000; 	// Register 18-1:
    ADCON2 = 0b0000010000000100;
    ADCHS = 0b0000000000000011;
    ADCSSL = 0b0000000000011000;		// Register 18-6: ADCSSL: A/D Input Scan Select Register
    ADCON3 = 0x0002;          
    
    ADCON1bits.ADON = 1;
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
    PR2=2342; //PERIODO DEL PWM (Tpwm/(TCY*PRESCALER))-1 -> Tpwm= [(PR2) + 1]*TCY*Prescaler
    TMR2=0; // DONDE ARRANCA EL TIMER 2
    OC1R=0; //TON PARA EL PRIMER CICLO
    OC1RS=0;
    OC1CONbits.OCM=0b110;
    OC1CONbits.OCTSEL=0;
    T2CONbits.T32=0; //SI QUEREMOS UN TIMER DE 32 O 16 BITS
    T2CONbits.TCKPS=0b11; //ELEGIMOS EL PRESCALER
    T2CONbits.TCS=0; //ELEGIMOS QUE QUEREMOS SEGUIR LAS INSTRUCCIONES INTERNAS COMO ORIGEN DEL TIMER
    T2CONbits.TON=1;//ENCENDEMOS EL TIMER2
}


void __attribute__ ((__interrupt__)) _ADCInterrupt(void)
{
    ADCON1bits.ADON = 0;   // this must be executed before the end of 17th conversion
    IFS0bits.ADIF = 0;
    AD = ADCBUF0;
    AD1 = ADCBUF1;
    ADCf = ((float)AD1*234)/4095.0;
    ADCint = (int)ADCf;
    writeshex(AD,AD1);
    ADCON1bits.ADON = 1;
}
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

void configuarttx(void)
{
    OSCTUN=0; 
    RCONbits.SWDTEN=0;
    U1MODEbits.UARTEN = 0;// Disable UART
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Pari vcxty, 8-data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1BRG = 15;           // UxBRG = Fcy/(16*BaudRate) - 1
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
/*
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
    U1TXREG=ADCHIGH;
    delay_us(50);
    U1TXREG=ADCLOW1;
    U1TXREG=ADCHIGH1;
    delay_us(50);
    U1TXREG=FRAMEL;
}*/

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