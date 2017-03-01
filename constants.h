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
#define period      8000000
#define step        (period/8)
#define S1_1        LATAbits.LATA4  //orange
#define S1_2        LATEbits.LATE1  //yellow
#define S1_3        LATEbits.LATE2  //pink
#define S1_4        LATCbits.LATC0  //blue
#define S2_1
#define S2_2

//analog input channels
#define tab         0
#define label       1
#define backlog     2
#define soup        3
#define soda        4

//logic
#define currMom()  time[0] + 60*time[1] + 3600*time[2]


#endif	/* CONSTANTS_H */
