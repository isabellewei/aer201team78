/*
 * File:   main.c
 * Author: Isabelle
 *
 * Created on February 4, 2017, 3:17 PM
 */
#include "configBits.h"
#include "constants.h"
#include "functions.h"

//global variables
const char keys[] = "123A456B789C*0#D";
unsigned char time[7];
volatile unsigned char keypress = NULL;

void readADC(char channel){
    // Select A2D channel to read
    ADCON0 = ((channel <<2));
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO_NOT_DONE){__delay_ms(5);} 
}



//these 2 functions use keypress variable
void keycheck(void){
    while(PORTBbits.RB1 == 0){
        // RB1 is the interrupt pin, so if there is no key pressed, RB1 will be 0
            // the PIC will wait and do nothing until a key press is signaled
        }
        keypress = (PORTB & 0xF0)>>4; // Read the 4 bit character code
        while(PORTBbits.RB1 == 1){
            // Wait until the key has been released
        }
    Nop();  //breakpoint b/c compiler optimizations
    return;
}
void keyinterrupt(void){
    if(PORTBbits.RB1 == 1){
                keypress = (PORTB & 0xF0)>>4; // Read the 4 bit character code
                while(PORTBbits.RB1 == 1){
                    // Wait until the key has been released
                }
            }
            Nop();  //breakpoint b/c compiler optimizations
}

void initialize(void){
    OSCCON = 0xF2; // Set internal oscillator to 8MHZ, and enforce internal oscillator operation
    OSCTUNEbits.PLLEN = 1; // Enable PLL for the internal oscillator, Processor now runs at 32MHZ

    TRISA = 0x00;   //All output
    TRISB = 0b11110010; // Set Keypad Pins as input, rest are output
    TRISC = 0x00011100; // -,-,-,I2C, I2C, DC motor, -, -
                        //Set I2C pins as input, rest are output
    TRISD = 0x00;   //All output mode
    TRISE = 0x00;   //RE0 and RE1 output

    PR2 = 0xff; // load with PWM period value (fixed at 1.953 kHz)
    CCP1CON = 0b00001100; // setup for PWM mode 5:4 are PWM Duty Cycle LSB
    CCPR1L = 0x00; // eight high bits of duty cycle
    T2CON = 0b00000101; // Timer2 On, 1:1 Post, 4x prescale
    
    LATA = 0b01010101;
    LATB = 0b01010101;
    LATC = 0b01010101;
    LATD = 0x00;
    LATE = 0x00;
    LATDbits.LATD0 = 1;
    LATDbits.LATD1 = 0;
    LATEbits.LATE0 = 1;
    LATEbits.LATE1 = 0;

    ADCON0 = 0x00;  //Disable ADC
    ADCON1 = 0xFF;  //Set PORTB to be digital instead of analog default

    initLCD();

    I2C_Master_Init(10000); //Initialize I2C Master with 100KHz clock
    
    //initialize ADC
    nRBPU = 0;
    ADCON0 = 00000001;  //Enable ADC
    ADCON1 = 0x0B;  //AN0 to AN3 used as analog input
    CVRCON = 0x00; // Disable CCP reference voltage output
    CMCONbits.CIS = 0;
    ADFM = 1;
}

int main(void) {
    initialize();   
    int standby = 1; //1=true, 0=false
    int s = 0;      //stepper motor counter
    int dc = 0;
    int startTime;
    __delay_ms(10);

    while(1){
        updateTime();

        if (standby){
            keypress = NULL;
            homescreen();
            keyinterrupt();
            
            if(keypress == 2){ //user selected 3:Start sorting
                standby = 0; //not standby
                lcd_clear();
                printf("Sorting cans...");
                lcd_newline();
                printf("Any key to stop");
                PWM1(100);
                dc = 1; //ON
                startTime = currMom();
            }
            else if(keypress == 1){ //user selected 2:Logs
                displayLogs();
            }

        }
        else{ //sorting mode
            keypress = NULL;
            keyinterrupt();
            if (keypress != NULL) {//stop sorting
                standby = 1;
                PWM1off();
                dc = 0; //OFF
            }
             
            readADC(backlog);
            
            
        }
    }
    return;
}
