/*
 * File:   main.c
 * Author: Isabelle
 *
 * Created on February 4, 2017, 3:17 PM
 */
#include "configBits.h"
#include "declarations.h"
#include "functions.h"

//global variables
const char keys[] = "123A456B789C*0#D";
unsigned char time[7];
volatile unsigned char keypress = NULL;



//these 2 functions use keypress variable


void initialize(void){
    OSCCON = 0b01110000; // Set internal oscillator to 8MHZ, and enforce internal oscillator operation
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
    int soupLoad = 1; //can waiting to be sorted?
    int sodaLoad = 1; //can waiting to be sorted?
    int canCheck;
    int prevSodaTime = 0;
    int prevSoupTime = 0; //time of beginning of current can sort
    int sodaState = 0; //0:wait, 1:close+check, 2:open, 3:sort, 
    int tabState = 0; //0:no, 1: yes
    int labelState = 0; //current can has label or not?
    //int sodaCheck;
    int s = 1;
    int soupSort; //sort soup  cans?
    int prevSoupLoad; //most recent soup can pass
    int S1mode = 0;
    
    unsigned long c = 0;  //fastStep counter
    
    __delay_ms(10);
    
    
    while(1){
        updateTime();
        updateS1(S1mode);
        readADC(IR3);
        if (ADRES >= 0x3d0){soupLoad = 1;}
        else {soupLoad = 0;}
        readADC(IR4);
        sodaLoad = ADRESH;
        
        if (standby == 1){
            
            keypress = NULL;
            homescreen();
            keyinterrupt();
          
            
            if(keypress == 2){ //3: Start sorting
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
            else if(keypress == 1){ //2:Logs
                //printf("asdf");
                displayLogs();
            }
            else if(keypress == 15 ) { //D: testing components
                standby = 2;   
                s = 1;
            }
            else if(keypress == 14 ) { //#: testing soup can path
                standby = 3;   
                PWM2(500);
                labelState = 0;
                s = 1;
                soupSort = 0;
            }
            else if(keypress == 13) { //0: testing soda can path
                standby = 4;   
                sodaState= 0;
                prevSodaTime = currMom();
                tabState = 0;
                s = 1;
            }

        }
        else if(standby == 2){ //testing components
            // <editor-fold defaultstate="collapsed" desc="Testing Components">
            lcd_clear();
            keypress = NULL; 
            
            if (s==1){
                printf("testing S f");
                S1mode = 1;
                updateS1(S1mode);
                S2forward();
                //S1forward();
                S3forward();
                keyinterrupt();
            }
            else if(s==2){
                printf("testing S b");                
                S1mode = 2;
                updateS1(S1mode);
                //S1backward();
                S2backward();
                S3backward();                
                keyinterrupt();
            }
            
            else if(s==3){
                printf("testing S1shake");                
                //S1shake(); 
                S1mode = 3;
                updateS1(S1mode);
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
                    printf("testing IR3");
                    readADC(IR3);
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
                
                keypress = NULL;
                while(keypress==NULL){
                    keypress = NULL;
                    lcd_clear();
                    printf("testing faststep");
                    
                    if (c == 1){S1_1 = 1;}
                    else{S1_1 = 0;} 
                    if (c == 2){S1_2 = 1;}
                    else{S1_2 = 0;}
                    if (c == 3){S1_3 = 1;}
                    else{S1_3 = 0;}
                    if (c == 4){S1_4 = 1;}
                    else{S1_4 = 0;}
                    
                    c++;
                    if (c>4) {s=1;} 
                    __delay_ms(250);
                    keyinterrupt();
                }
                
                c = 0;
                keypress = NULL;
                while(keypress==NULL){
                    keypress = NULL;
                    c++;
                    if((c %10000)== 0){
                        lcd_clear();
                        printf("program speed");
                        lcd_newline();
                        printf("loops: %lu", c);
                    }
                            
                    keyinterrupt();
                }
                
                standby = 1;
            }                     
                                   
            if(keypress != NULL){
                s++;
            }
        // </editor-fold>    
        } 
        else if(standby == 3){ //testing soup can path
            // <editor-fold defaultstate="collapsed" desc="Testing soup can path">
            lcd_clear();
            printf("soup can test");
            
            if(soupLoad){
                prevSoupLoad = currMom();
                if (!soupSort){
                    soupSort = 1;
                    prevSoupTime = currMom();
                    __delay_ms(250);
                }
            }   
            
            if (soupSort){

                if (timePassed(prevSoupTime) < 5){
                    S1mode = 3;
                    updateS1(S1mode);
                    //S1shake();
                    
                    readADC(label);                
                    if(ADRESL<=0x1){labelState = 1;} //no label 
                    lcd_clear();
                    printf("value: %x", ADRESL);
                    lcd_newline();
                    printf("testing %d", timePassed(prevSoupTime));
                }
                else if (labelState == 1){
                    keypress = NULL;
                    keyinterrupt();
                    if (keypress != NULL) {//stop sorting
                        standby = 1;
                    }

                    lcd_clear();
                    printf("no label");
                    lcd_newline();
                    if(timePassed(prevSoupTime) < 13){
                        //S1forward();
                        S1mode = 1;
                        updateS1(S1mode);
                        printf("sorting %d", timePassed(prevSoupTime));
                    }
                    else if(timePassed(prevSoupTime) < 21){
                        //S1backward();
                        S1mode = 2;
                        updateS1(S1mode);
                        printf("returning %d", timePassed(prevSoupTime));
                    }
                    else{labelState = 100;}
                }
                else if(labelState == 0){
                    lcd_clear();
                    printf("label");
                    lcd_newline();
                    if(timePassed(prevSoupTime) < 12){
                        //S1backward();
                        S1mode = 2;
                        updateS1(S1mode);
                        printf("sorting %d", timePassed(prevSoupTime));
                    }
                    else if(timePassed(prevSoupTime) < 19){
                        //S1forward();
                        S1mode = 1;
                        updateS1(S1mode);
                        printf("returning %d", timePassed(prevSoupTime));
                    }
                    else{labelState = 100;}
                }
                else{
                    lcd_clear();
                    printf("finished %d", timePassed(prevSoupTime));
                    prevSoupTime = currMom();
                    labelState = 0;
                    if (timePassed(prevSoupLoad) > 9) {soupSort = 0;}
                }
            }
            else{
                //S1shake();
                S1mode = 3;
                updateS1(S1mode);
            }
        // </editor-fold>           
        } 
        else if(standby == 4){ //testing soda can path
            // <editor-fold defaultstate="collapsed" desc="Testing soda can path">
            switch(sodaState){
                case 0: //wait
                    S1off();
                    S2off();
                    if(sodaLoad){
                        prevSodaTime = currMom();
                        sodaState ++;
                    }
                case 1: //close+check+open
                    S2off();
                    if(timePassed(prevSodaTime) < 3){
                        S1backward(); //close
                    }
                    else if(timePassed(prevSodaTime) < 4){
                        S1off();
                        readADC(tab); //check
                        tabState = ADRESH;

                    }
                    else if(timePassed(prevSodaTime) < 7){
                        S1forward(); //open
                    }
                    else{
                        prevSodaTime = currMom();
                        sodaState ++;
                    }
                case 2: //sort
                    S1off();
                    if(timePassed(prevSodaTime) < 3){
                        if(tabState){S2forward();}
                        else{S2backward();}
                    }
                    else if(timePassed(prevSodaTime) < 6){
                        if(tabState){S2backward();}
                        else{S2forward();}
                    }
                    else{
                        sodaState = 0;
                    } 
                default:
                    sodaState = 0;
                    break;
            }
            // </editor-fold>
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
