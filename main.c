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
    ADCON0bits.ADON = 1;
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
            //lcd_home();
            //printf("asdf");
            
        }
        //lcd_clear();
        //printf("%c", keypress);
        //lcd_newline();
        //printf("%x", PORTB & 0xF0);
        //__delay_ms(2000);
    }
    Nop();  //breakpoint b/c compiler optimizations
    
}

void initialize(void){
    OSCCON = 0xF2; // Set internal oscillator to 8MHZ, and enforce internal oscillator operation
    OSCTUNEbits.PLLEN = 1; // Enable PLL for the internal oscillator, Processor now runs at 32MHZ

    TRISA = 0b11001111;   //All output
    TRISB = 0b11110010; // Set Keypad Pins as input, rest are output
    TRISC = 0b00011000; // -,-,-,I2C, I2C, DC motor, -, -
                        //Set I2C pins as input, rest are output
    TRISD = 0x00;   //All output mode
    TRISE = 0b00000001;   //RE0 and RE1 output

    PR2 = 0xff; // load with PWM period value (fixed at 1.953 kHz)
    CCP1CON = 0b00001100; // setup for PWM mode 5:4 are PWM Duty Cycle LSB
    CCPR1L = 0x00; // eight high bits of duty cycle
    CCP2CON = 0b00001100; // setup for PWM mode 5:4 are PWM Duty Cycle LSB
    CCPR2L = 0x00; // eight high bits of duty cycle
    T2CON = 0b00000101; // Timer2 On, 1:1 Post, 4x prescale
    
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;
    
    initLCD();

    I2C_Master_Init(10000); //Initialize I2C Master with 100KHz clock
    
    //initialize ADC
    nRBPU = 0;
    ADCON0 = 00000001;  //Enable ADC
    ADCON1 = 0b00001001;  //AN0 to AN5 used as analog input
    CVRCON = 0x00; // Disable CCP reference voltage output
    CMCONbits.CIS = 0;
    ADFM = 1;
    
    T0CON = 0b11011000; //ON, 8-bit, internal clock, rising edge, prescale, 010 prescale value
}

int main(void) {
    initialize();   
    int standby = 1; //1=true, 0=false
    int s = 0;      //stepper motor counter
    int dc = 0;
    int startTime;
    int prevUnblock = 0;
    int soupLoad = 1;
    int sodaLoad = 1;
    int canCheck;
    int prevSodaTime = 0;
    int prevSoupTime = 0;
    int sodaState = 0; //0:wait, 1:close+check, 2:open, 3:sort, 
    int tabState = 0; //0:no, 1: yes
    int labelState = 0;
    //int sodaCheck;
    int s = 1;
    
    __delay_ms(10);
    
    
    while(1){
        updateTime();
        updateStepper();
        readADC(IR3);
        soupLoad = ADRESH;
        readADC(IR4);
        sodaLoad = ADRESH;
        
        if (standby == 1){
            
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
                canCheck = startTime;
                //soupCheck = startTime;
            }
            else if(keypress == 1){ //user selected 2:Logs
                //printf("asdf");
                displayLogs();
            }
            else if(keypress != NULL ) { //testing mode
                standby = keypress;   
                prevSoupTime = currMom();
                labelState = 0;
                s = 1;
            }

        }
        else if(standby == 15){ //D
            lcd_clear();
            keypress = NULL; 
            if (s==1){
                printf("testing S f");
                S2forward();
                S1forward();
                S3forward();
                keyinterrupt();
            }
            else if(s==2){
                printf("testing S b");                
                S1backward();
                S2backward();
                S3backward();                
                keyinterrupt();
            }
            else if(s==3){
                printf("testing S b");                
                S3off(); 
                S2off(); 
                S1off(); 
                keyinterrupt();
            }
            
            else{
                
                lcd_clear();
                printf("testing PWM2");
                PWM2(500);
                keycheck();
                PWM2off();
                
                lcd_clear();
                printf("testing PWM1");
                PWM1(500);
                keycheck();
                PWM1off();
                
                keypress = NULL;
                startTime = currMom();
                while(keypress==NULL){
                    updateTime();   
                    keypress = NULL;
                    __delay_ms(50);
                    lcd_clear();
                    printf("time %x %x", currMom(), timePassed(startTime));
                    keyinterrupt();
                }
                
                keypress = NULL;
                while(keypress==NULL){
                    keypress = NULL;
                    __delay_ms(50);
                    lcd_clear();
                    printf("testing IR1");
                    readADC(IR1);
                    lcd_newline();
                    printf("%x", ADRES);
                    keyinterrupt();
                }
                
                keypress = NULL;
                while(keypress==NULL){
                    keypress = NULL;
                    __delay_ms(50);
                    lcd_clear();
                    printf("testing IR2");
                    readADC(IR2);
                    lcd_newline();
                    printf("%x", ADRES);
                    keyinterrupt();
                }
                
                keypress = NULL;
                while(keypress==NULL){
                    keypress = NULL;
                    __delay_ms(50);
                    lcd_clear();
                    printf("testing label");
                    readADC(label);
                    lcd_newline();
                    printf("%x   %x", ADRESH, ADRESL);
                    keyinterrupt();
                }
                
                
                
                
                standby = 1;
            }
                        
            if(keypress != NULL){
                s++;
            }
            
        } 
        else if(standby == 14){ //#
            lcd_clear();
            printf("soup can test");
            if (timePassed(prevSoupTime) < 3){
                
                S1off();
                readADC(label);                
                if(ADRESL<=0x1){labelState = 1;} //no label 
                lcd_newline();
                printf("value: %x", ADRESL);
                lcd_clear();
                printf("testing %d", timePassed(prevSoupTime));
            }
            else if (labelState == 1){
                lcd_clear();
                printf("no label");
                lcd_newline();
                if(timePassed(prevSoupTime) < 8){
                    S1forward();
                    printf("sorting %d", timePassed(prevSoupTime));
                }
                else if(timePassed(prevSoupTime) < 13){
                    S1backward();
                    printf("returning %d", timePassed(prevSoupTime));
                }
                else{labelState = 100;}
            }
            else if(labelState == 0){
                lcd_clear();
                printf("label");
                lcd_newline();
                if(timePassed(prevSoupTime) < 8){
                    S1backward();
                    printf("sorting %d", timePassed(prevSoupTime));
                }
                else if(timePassed(prevSoupTime) < 13){
                    S1forward();
                    printf("returning %d", timePassed(prevSoupTime));
                }
                else{labelState = 100;}
            }
            else{
                lcd_clear();
                printf("finished %d", timePassed(prevSoupTime));
                prevSoupTime = currMom();
                labelState = 0;
            }
                   
        }            
        else{ //sorting mode
            lcd_clear();
            printf("sorting");
            keypress = NULL;
            keyinterrupt();
            if (keypress != NULL) {//stop sorting
                standby = 1;
            }
             
            /*
            readADC(backlog);
            if (dc ^ backlogTest(prevUnblock, dc)){
                dc = backlogTest(prevUnblock, dc);
                if (dc){PWM1(500);}
                else {
                    PWM1off();
                    prevUnblock = currMom();
                }
            }
             */
            
            if(timePassed(canCheck) > 10){
                readADC(IR4);
                soupLoad = ADRESH;
                readADC(IR3);
                sodaLoad = ADRESH;
                if (!soupLoad & !sodaLoad){
                    standby = 1;
                }
                canCheck = currMom();
            }
             
            
            if(standby==1){
                lcd_clear();
                printf("Finished sorting! ");
                __delay_ms(500);
                PWM1off();
                dc = 0; //OFF
                updateEEPROM(timePassed(startTime));
            }
        }
    }
    return;
}
