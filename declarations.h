/*
 * File:   constants.h
 * Author: Administrator
 *
 * Created on August 11, 2016, 2:41 PM
 */

#ifndef DECLARATIONS_H
#define	DECLARATIONS_H         //Prevent multiple inclusion

#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

#define _XTAL_FREQ 32000000      // Define osc freq for use in delay macros
#define TMR2PRESCALE 4

extern unsigned char time[7]; 
extern volatile unsigned char keypress;
extern int t;
extern int t2;
extern int t3;


//LCD Control Registers
#define RS          LATDbits.LATD2
#define E           LATDbits.LATD3
#define	LCD_PORT    LATD   //On LATD[4,7]
#define LCD_DELAY   25
//LCD macros
#define lcd_newline() lcdInst(0b11000000);
#define lcd_clear() lcdInst(0b00000001);  __delay_ms(5);
#define lcd_home() lcdInst(0b10000000);
#define bcd_to_num(num) (num & 0x0F) + ((num & 0xF0)>>4)*10

//Stepper Motors
#define step        1
#define S1_1        LATDbits.LATD1  
#define S1_2        LATBbits.LATB3  
#define S1_3        LATBbits.LATB2  
#define S1_4        LATBbits.LATB0  

#define S3_1        LATCbits.LATC7
#define S3_2        LATCbits.LATC6
#define S3_3        LATCbits.LATC0
#define S3_4        LATCbits.LATC5

#define S2_1        LATEbits.LATE2
#define S2_2        LATEbits.LATE1
#define S2_3        LATAbits.LATA4
#define S2_4        LATDbits.LATD0

//analog input channels
#define tab         3
#define label       4   //cond1
#define IR1         1
#define IR2         0
#define IR3         2   //soupLoad
#define IR4         5

//logic
#define currMom()  (bcd_to_num(time[0]) + 60*bcd_to_num(time[1]) + 3600*bcd_to_num(time[2]))


#endif	/* DECLARATIONS_H */
