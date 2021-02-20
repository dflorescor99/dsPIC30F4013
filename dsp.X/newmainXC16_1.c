/*
 * File:   newmainXC16.c
 * Author: root
 *
 * Created on July 5, 2020, 2:12 AM
 */


#include "xc.h"
#include <p30F4013.h>
#include <stdio.h>
#define MIPS 30
#define EN LATDbits.LATD2
#define RS LATDbits.LATD3
#define D4 LATBbits.LATB9
#define D5 LATBbits.LATB10
#define D6 LATBbits.LATB11
#define D7 LATBbits.LATB12

_FOSC(CSW_FSCM_OFF & FRC_PLL16);
_FWDT(WDT_OFF);

void delay_ms (unsigned long delay_count);
void delay_us (unsigned int delay_count);
void Lcd_Port(char a);
void Lcd_Cmd(char a);
void Lcd_Clear();
void Lcd_Set_Cursor(char a, char b);
void Lcd_Init();
void Lcd_Write_Char(char a);
void Lcd_Write_String(char *a);
void Lcd_Shift_Right();
void Lcd_Shift_Left();

int main(void) {
    ADPCFG = 0b1111111111111111; 
    TRISBbits.TRISB9 =0;
    TRISBbits.TRISB10 = 0; 
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB12 = 0;
    TRISDbits.TRISD3 = 0; 
    TRISDbits.TRISD2 = 0;
    char h[30];
    sprintf(h,"holaa");
    Lcd_Init();
    Lcd_Clear();
Lcd_Set_Cursor(1,1);
    Lcd_Write_String(h);
    delay_ms(1000);
    while(1)
    {
        Lcd_Clear();
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String(h);
    }
    return 0;
}

void Lcd_Port(char a)
{
    //SI LE PASAMOS UN BINARIO, PARA LOS ULTIMOS 4 BITS
    //A RD7, RD6, RD5, RD4 RESPECTIVAMENTE, QUE SON 
    //LOS BITS DE DATA QUE PUSIMOS EN EL PIC
	if(a & (1))
		D4 = 1;
	else
		D4 = 0;

	if(a & (0b00000010))
		D5 = 1;
	else
		D5 = 0;

	if(a & (0b00000100))
		D6 = 1;
	else
		D6 = 0;

	if(a & (0b00001000))
		D7 = 1;
	else
		D7 = 0;
}
void Lcd_Cmd(char a)
{
	RS = 0;
    //RS = 0, ESTO QUIERE DECIR QUE LE ESTAMOS DANDO UNA ORDEN
    //NO LE ESTAMOS PASANDO CARACTERES
	Lcd_Port(a);
    //
	EN  = 1;             // => E = 1
        delay_ms(4);
        EN  = 0;             // => E = 0
    //SUBIMOS Y BAJAMOS (1=>0) A EN PARA QUE EL VALOR QUEDE GRABADO
}

void Lcd_Clear()
{
	Lcd_Cmd(0);
	Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b)
{
	char temp,z,y;
	if(a == 1)
	{
	  temp = (0b10000000) + b - 1;
		z = temp>>4;
		y = temp & (0b00001111);
		Lcd_Cmd(z);
		Lcd_Cmd(y);
	}
	else if(a == 2)
	{
		temp = (0b11000000) + b - 1;
		z = temp>>4;
		y = temp & (0b00001111);
		Lcd_Cmd(z);
		Lcd_Cmd(y);
	}
}

void Lcd_Init()
{
  Lcd_Port(0b00000000);
  delay_ms(20);
  Lcd_Cmd(0b00000011);
  delay_ms(5);
  Lcd_Cmd(0b00000011);
  delay_ms(11);
  Lcd_Cmd(0b00000011);
  /////////////////////////////////////////////////////
  Lcd_Cmd(0b00000010);//PORQUE LO PONEMOS 2 VECES??
  Lcd_Cmd(0b00000010);
  Lcd_Cmd(0b00001000);
  Lcd_Cmd(0b00000000);
  Lcd_Cmd(0b00001100);
  Lcd_Cmd(0b00000000);
  Lcd_Cmd(0b00000110);
}

void Lcd_Write_Char(char a)//PRIMERO PASAMOS LA PARTE ALTA
{
   char CHAR_L,CHAR_H;
   CHAR_L = a & (0b00001111);
   CHAR_H = a & (0b11110000);
   RS = 1;             // => RS = 1 SIGNIFICA QUE PONEMOS EL LCD EN MODO LEER DATOS
   Lcd_Port(CHAR_H>>4);             //Data transfer
   EN = 1;
   delay_us(40);
   EN = 0;
   Lcd_Port(CHAR_L);
   EN = 1;
   delay_us(40);
   EN = 0;
}

void Lcd_Write_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	   Lcd_Write_Char(a[i]);
}

void Lcd_Shift_Right()
{
	Lcd_Cmd(0b00000001);
	Lcd_Cmd(0b00001100);
}

void Lcd_Shift_Left()
{
	Lcd_Cmd(0b00000001);
	Lcd_Cmd(0b00001000);
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