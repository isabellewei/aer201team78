/* 
 * File:   functions.h
 * Author: Isabelle
 *
 * Created on February 20, 2017, 11:36 PM
 */

#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

//LCD
void lcdInst(char data);
void lcdNibble(char data);
void initLCD(void);

//I2C
void I2C_Master_Init(const unsigned long c);
void I2C_Master_Wait();
void I2C_Master_Start();
void I2C_Master_RepeatedStart();
void I2C_Master_Write(unsigned d);
unsigned char I2C_Master_Read(unsigned char a);
void I2C_Master_Stop();
void delay_10ms(unsigned char n);
void set_time(void);
void updateTime(void);

//motors
void PWM1(int duty);
void PWM1off();

//interface
signed char eepromRead(signed char address);
void eepromWrite(signed char address, signed char data);
void displayLogs(void);
void homescreen(void);
void updateEEPROM(int time);

//logic
int backlogTest(int blockedTime, int motorStatus);
int timePassed(int start);

#endif	/* FUNCTIONS_H */

