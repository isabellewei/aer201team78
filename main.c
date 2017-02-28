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
const char currtime[7] = {  0x30, //45 Seconds
                        0x11, //59 Minutes
                        0x20, //24 hour mode, set to 8pm
                        0x03, //Tuesday
                        0x07, //07th
                        0x02, //February
                        0x17};//2017
unsigned char time[7];
volatile unsigned char keypress = NULL;

void homescreen(void){
    lcd_home();
    printf("%02x/%02x/%02x ", time[6],time[5],time[4]);    //Print date in YY/MM/DD
    printf("3:Start");
    lcd_newline();
    printf("%02x:%02x:%02x", time[2],time[1],time[0]);    //HH:MM:SS
    printf(" 2:Logs");
}

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

void set_time(void){
    I2C_Master_Start(); //Start condition
    I2C_Master_Write(0b11010000); //7 bit RTC address + Write
    I2C_Master_Write(0x00); //Set memory pointer to seconds
    for(char i=0; i<7; i++){
        I2C_Master_Write(currtime[i]);
    }
    I2C_Master_Stop(); //Stop condition
}

void readADC(char channel){
    // Select A2D channel to read
    ADCON0 = ((channel <<2));
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO_NOT_DONE){__delay_ms(5);} 
}

int main(void) {
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
    
    int standby = 1; //1=true, 0=false
    int s = 0;      //stepper motor counter

    __delay_ms(10);

    while(1){
        LATA = 00000000;

        //Reset RTC memory pointer
        I2C_Master_Start(); //Start condition
        I2C_Master_Write(0b11010000); //7 bit RTC address + Write
        I2C_Master_Write(0x00); //Set memory pointer to seconds
        I2C_Master_Stop(); //Stop condition

        //Read Current Time
        I2C_Master_Start();
        I2C_Master_Write(0b11010001); //7 bit RTC address + Read
        for(unsigned char j=0;j<0x06;j++){
            time[j] = I2C_Master_Read(1);
        }
        time[6] = I2C_Master_Read(0);       //Final Read without ack
        I2C_Master_Stop();

        if (standby){
            keypress = NULL;
            homescreen();
            keyinterrupt();
            //lcd_clear();

            if(keypress == 2){ //user selected 3:Start sorting
                standby = 0; //not standby
                lcd_clear();
                printf("Sorting cans...");
                lcd_newline();
                printf("Any key to stop");
                PWM1(100);
            }
            else if(keypress == 1){ //user selected 2:Logs
                lcd_clear();
                printf("Run#1    A:Next");
                lcd_newline();
                printf("# of cans: 10");
                keycheck();
                lcd_clear();
                printf("Run#1    A:Next");
                lcd_newline();
                printf("# soda cans: 6");
                keycheck();
                lcd_clear();
                printf("Run#1    A:Next");
                lcd_newline();
                printf("# soup cans: 4");
                keycheck();
            }

        }
        else{ //sorting mode
            keypress = NULL;
            keyinterrupt();
            if (keypress != NULL) {//stop sorting
                standby = 1;
                PWM1off();
            }

           
            
        }
    }
    return;
}
