/*
 * File:   Final_timer.c
 * Author: root
 *
 * Created on February 22, 2021, 1:52 AM
 */

// FOSC
#pragma config FOSFPR = FRC_PLL16       // Oscillator (FRC w/PLL 16x)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)
// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)
// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = BORV20          // Brown Out Voltage (Reserved)
#pragma config BOREN = PBOR_ON          // PBOR Enable (Enabled)
#pragma config MCLRE = MCLR_EN          // Master Clear Enable (Enabled)
// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)
// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

#include <p30F4013.h>
#include <stdio.h>
#include "xc.h"

#define MIPS 30


void conf_uart(void);
void conf_adc(void);
void conf_timer(void);
void conf_pwm(void);
void conf_CN(void);
void send_uart(uint8_t A);
void writeshex(uint8_t A, uint16_t B, uint16_t C);
void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);

unsigned int set_value=0;
float auxf=0.0;
float coef[4]={1.0,1.0,4.0,-3.92};
float val[4]={0.0,0.0,0.0,0.0};
float Y=0.0;
uint16_t pwm_duty=0;
uint16_t aux=0;
uint16_t ADCValue0=0;
uint16_t ADCValue1=0;
uint8_t signal=0x00;

int main(void) {
    
    TRISBbits.TRISB4 = 1;
    
    conf_uart();
    conf_adc();
    conf_pwm();
    conf_CN();
    conf_timer();
    
    delay_ms(7000);
    
    ADCON1bits.ADON = 1;
    ADCON1bits.ASAM = 1;
    while (1)
    {
        while (!IFS0bits.ADIF);
        ADCValue0 = ADCBUF0;
        ADCValue1 = ADCBUF1;
        signal=~signal;
        auxf=(aux-ADCValue1)*1.0;
        val[2]=auxf;
        val[0]=coef[1]*val[1]+coef[2]*val[2]+coef[3]*val[3];
        auxf=val[0]*3000.0/4095.0;
        
        val[1]=val[0];
        val[3]=val[2];
        
        pwm_duty=(int)auxf;
        if (aux==0){
            pwm_duty=0;
            val[0]=0.0,val[1]=0.0,val[2]=0.0,val[3]=0.0;
        }
        OC1RS=pwm_duty;
        writeshex(signal,ADCValue0,ADCValue1);
        IFS0bits.ADIF = 0;
    }
    return 0;
}

void conf_pwm(void)
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

void conf_uart(void)
{
    OSCTUN=0; 
    RCONbits.SWDTEN=0;
    U1MODEbits.UARTEN = 0;// Disable UART
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud Disabled
    U1BRG = 15;           // UxBRG = Fcy/(16*BaudRate) - 1, elegimos un Baudrate de 115200
    U1STAbits.UTXISEL = 0;         // Interrupcion cuando mandamos un caracter
    U1STAbits.URXISEL = 0;         // Interrupcion cuando recibimos un caracter
    U1MODEbits.UARTEN = 1;         // Enable UART
    U1STAbits.UTXEN = 1;           // Enable UART Tx, solo activamos el envio
}

void conf_adc(void)
{
    ADPCFG = 0b1111111111110011; //Ponemos todas las entradas como digitales menos AN2(RB2) y AN3(RB3)
    ADCON1 = 0b0000000001000000;
    ADCON3 = 0b0000000000010011;
    ADCON2 = 0b0000010000000100;
    ADCHS  = 0b0000000000000000;
    ADCSSL = 0b0000000000001100;

    //SSRC bit = 010 implies GP TMR3
    //compare ends sampling and starts
    //converting.
    //Connect RB2/AN2 as CH0 input ..
    //in this example RB2/AN2 is the input
    // Sample time is TMR3, Tad = internal Tcy/2
    // Interrupcion despues de una conversion
    // set TMR3 to time out every 100 mSecs
    
}

void conf_CN(void)
{
    CNEN1bits.CN6IE = 1; // Enable CN3 pin for interrupt detection
    CNPU1bits.CN6PUE=1;
    IEC0bits.CNIE = 1; // Enable CN interrupts
    IFS0bits.CNIF = 0; // Reset CN interrupt
}

void conf_timer(void)
{
    TMR3 = 0;
    PR3= 37499;
    T3CON = 0b1000000000010000;
}

void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    IFS0bits.CNIF = 0;
    CNEN1bits.CN6IE = 0;
    if (~LATBbits.LATB4){
        delay_ms(200);
        if (set_value==0){
            aux=ADCValue0;// cambiar a 3 despues
        }
        else{
            
            aux=0;
        }
        set_value=~set_value;
    }
    CNEN1bits.CN6IE = 1;
}

void send_uart(uint8_t A)
{    
    U1TXREG=A;
    delay_us(50);
}

void writeshex(uint8_t A, uint16_t B, uint16_t C)
{
    uint16_t mask=0x00FF;
    uint8_t FRAMEH=0x03;
    uint8_t FRAMEL=0xFC;
    uint8_t ADCLOW1=mask&B;
    uint8_t ADCHIGH1=mask&(B>>8);
    uint8_t ADCLOW=mask&C;
    uint8_t ADCHIGH=mask&(C>>8);
    
    U1TXREG=FRAMEH;
    delay_us(50);
    U1TXREG=A;
    delay_us(50);
    U1TXREG=ADCLOW1;
    delay_us(50);
    U1TXREG=ADCHIGH1;
    delay_us(50);
    U1TXREG=ADCLOW;
    delay_us(50);
    U1TXREG=ADCHIGH;
    delay_us(50);
    U1TXREG=mask&aux;
    delay_us(50);
    U1TXREG=mask&(aux>>8);
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