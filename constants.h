/*
 * File:   constants.h
 * Author: Administrator
 *
 * Created on August 11, 2016, 2:41 PM
 */

#ifndef CONSTANTS_H
#define	CONSTANTS_H         //Prevent multiple inclusion

#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

#define _XTAL_FREQ 32000000      // Define osc freq for use in delay macros
#define TMR2PRESCALE 4

extern unsigned char time[7]; 


//LCD Control Registers
#define RS          LATDbits.LATD2
#define E           LATDbits.LATD3
#define	LCD_PORT    LATD   //On LATD[4,7]
#define LCD_DELAY   25
//LCD macros
#define lcd_newline() lcdInst(0b11000000);
#define lcd_clear() lcdInst(0b00000001);  __delay_ms(10);
#define lcd_home() lcdInst(0b10000000);
#define bcd_to_num(num) (num & 0x0F) + ((num & 0xF0)>>4)*10

//Stepper Motors
#define step        50
#define S1_1        LATDbits.LATD1  
#define S1_2        LATBbits.LATB3  
#define S1_3        LATBbits.LATB2  
#define S1_4        LATBbits.LATB0  

#define S2_1        LATCbits.LATC7
#define S2_2        LATCbits.LATC6
#define S2_3        LATCbits.LATC0
#define S2_4        LATCbits.LATC5

#define S3_1        LATEbits.LATE2
#define S3_2        LATEbits.LATE1
#define S3_3        LATAbits.LATA4
#define S3_4        LATDbits.LATD0

//analog input channels
#define tab         4
#define label       3
#define IR1         0
#define IR2         1
#define soup        2
#define soda        5

//logic
#define currMom()  time[0] + 60*time[1] + 3600*time[2]


#endif	/* CONSTANTS_H */
