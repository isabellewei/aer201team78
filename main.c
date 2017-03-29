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
    //OSCCON = 0b01110000; // Set internal oscillator to 8MHZ, and enforce internal oscillator operation
    OSCCON = 0xF0; //8MHz
    OSCTUNEbits.PLLEN = 1; // Enable PLL for the internal oscillator, Processor now runs at 32MHZ

    TRISA = 0b11101111;   //All output
    TRISB = 0b11110010; // Set Keypad Pins as input, rest are output
    TRISC = 0b00000000; // -,-,-,I2C, I2C, DC motor, -, -
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
    ADCON2 = 0b10001010;
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
    //int s = 1;
    int soupSort; //sort soup  cans?
    int prevSoupLoad; //most recent soup can pass
    int sodaSort; //sort soup  cans?
    int prevSodaLoad; //most recent soup can pass
    int S1mode = 0;
    int S2mode = 0;
    int S3mode = 0;
    int S1steps;
    int S3steps;
    int firstrun;
    int sodaThresh;
    
    unsigned long c = 0;  //fastStep counter
    
    __delay_ms(10);
    
    
    while(1){
        updateTime();
        updateS1(S1mode);
        updateS2(S2mode);
        updateS3(S3mode);
        //7__delay_ms(5);
        readADC(IR3);
        if (ADRES >= 0x3d0){soupLoad = 1;}
        else {soupLoad = 0;}
        
        
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
                
                dc = 1; //ON
                startTime = currMom();
                canCheck = startTime;
                PWM2(600);
                //PWM1(500);
                labelState = 0;
                soupSort = 0;
                prevSodaTime = currMom();
                tabState = 0;
                sodaSort = 0;
                for(s=0;s<40;s++){
                    S2mode = 2; //arm out
                    updateS2(S2mode);
                }
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
                //PWM1(500);
                labelState = 0;
                //s = 1;
                soupSort = 0;
            }
            else if(keypress == 10) { //0: testing soda can path
                standby = 4;   
                //sodaState= 0;
                prevSodaTime = currMom();
                tabState = 0;
                sodaSort = 0;
                startTime = currMom();
                //s = 1;
                for(s=0;s<40;s++){
                    S2mode = 2; //arm out
                    updateS2(S2mode);
                }
      
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
                S2mode = 1;
                updateS2(S2mode);
                S3mode = 1;
                updateS3(S3mode);                
                keyinterrupt();
            }
            else if(s==2){
                printf("testing S b");                
                S1mode = 2;
                updateS1(S1mode);
                S2mode = 2;
                updateS2(S2mode);
                S3mode = 2;
                updateS3(S3mode);                              
                keyinterrupt();
            }
            
            else if(s==3){
                printf("testing S shake");                
                S1mode = 3;
                updateS1(S1mode);
                S2mode = 3;
                updateS2(S2mode);
                S3mode = 3;
                updateS3(S3mode);
                keyinterrupt();
            }

            else{                
                keypress = NULL;
                lcd_clear();
                printf("testing S1 pins");
                while(keypress!=3){ //A to stop
                    //keypress = NULL;
                    keyinterrupt();
                    
                    if (keypress == 2){S1_1 = 1;}
                    else{S1_1 = 0;} 
                    if (keypress == 6){S1_2 = 1;}
                    else{S1_2 = 0;}
                    if (keypress == 10){S1_3 = 1;}
                    else{S1_3 = 0;}
                    if (keypress == 14){S1_4 = 1;}
                    else{S1_4 = 0;}        
                }
                
                keypress = NULL;
                lcd_clear();
                printf("testing S2 pins");
                while(keypress!=3){ //A to stop
                    //keypress = NULL;
                    keyinterrupt();
                    
                    if (keypress == 2){S2_1 = 1;}
                    else{S2_1 = 0;} 
                    if (keypress == 6){S2_2 = 1;}
                    else{S2_2 = 0;}
                    if (keypress == 10){S2_3 = 1;}
                    else{S2_3 = 0;}
                    if (keypress == 14){S2_4 = 1;}
                    else{S2_4 = 0;}        
                }
                
                keypress = NULL;
                lcd_clear();
                printf("testing S3 pins");
                while(keypress!=3){ //A to stop
                    //keypress = NULL;
                    keyinterrupt();
                    
                    if (keypress == 2){S3_1 = 1;}
                    else{S3_1 = 0;} 
                    if (keypress == 6){S3_2 = 1;}
                    else{S3_2 = 0;}
                    if (keypress == 10){S3_3 = 1;}
                    else{S3_3 = 0;}
                    if (keypress == 14){S3_4 = 1;}
                    else{S3_4 = 0;}        
                }

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
                    __delay_ms(50);
                    lcd_clear();
                    printf("testing tab");
                    readADC(tab);
                    lcd_newline();
                    printf("%x   %x", ADRESH, ADRESL);
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
            lcd_newline();
            printf("%d  %d", S1mode, soupSort);
            
            if(soupLoad){
                prevSoupLoad = currMom();
                if (!soupSort){
                    soupSort = 1;
                    //prevSoupTime = currMom();
                    S1steps = 0;
                    __delay_ms(150);
                }
            }   
            
            if (soupSort){
                //__delay_ms(10);
                S1steps ++;

                if (S1steps < 20){
                    S1mode = 3;
                    updateS1(S1mode);
                    //S1shake();
                    
                    readADC(label);                
                    if(ADRESL<=0x1){labelState = 1;} //no label 
                    lcd_clear();
                    printf("value: %x", ADRESL);
                    lcd_newline();
                    printf("testing %d", S1steps);
                }
                else if (labelState == 1){
                    //keypress = NULL;
                    //keyinterrupt();
                    //if (keypress != NULL) {//stop sorting
                    //    standby = 1;
                    //}

                    lcd_clear();
                    printf("no label");
                    lcd_newline();
                    if(S1steps < (20+30)){
                        S1mode = 1;
                        updateS1(S1mode);
                        printf("sorting %d", S1steps);
                    }
                    else if(S1steps < (20+30+29)){
                        //S1backward();
                        S1mode = 2;
                        updateS1(S1mode);
                        printf("returning %d", S1steps);
                    }
                    else{labelState = 100;}
                }
                else if(labelState == 0){
                    lcd_clear();
                    printf("label");
                    lcd_newline();
                    if(S1steps < (20+26)){
                        //S1backward();
                        S1mode = 2;
                        updateS1(S1mode);
                        printf("sorting %d", S1steps);
                    }
                    else if(S1steps < (20+26+26)){
                        //S1forward();
                        S1mode = 1;
                        updateS1(S1mode);
                        printf("returning %d", S1steps);
                    }
                    else{labelState = 100;}
                }
                else{
                    lcd_clear();
                    printf("finished %d", S1steps);
                    //prevSoupTime = currMom();
                    S1steps = 0;
                    labelState = 0;
                    if (timePassed(prevSoupLoad) > 2) {soupSort = 0;}
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
            lcd_clear();
            printf("soda can test");
            
            
            readADC(IR2);            
            if(ADRES >= 0x3b0){
                prevSodaLoad = currMom();
                if (!sodaSort && (t3== 1 || t3==8)){
                    sodaSort = 1;
                    //prevSoupTime = currMom();
                    //S2steps = 0;
                    S3steps = 0;
                    //__delay_ms(300);
                }
                if(timePassed(startTime) < 1){sodaSort = 0;}
            }   
                        
            lcd_newline();
            printf("%d %x %d %x", sodaSort, ADRES, prevSodaLoad, sodaThresh);
            
            if (sodaSort){
                //__delay_ms(10);
                //S2steps ++;
                S3steps ++;

                if(S3steps < 24){
                    S3mode = 3; //motor shake
                    updateS3(S3mode);   
                    S2mode = 4; //arm steady
                }
                else if (S3steps < (24+22)){      //go to sorting position
                    S3mode = 1; //motor outwards
                    updateS3(S3mode);   
                    S2mode = 4; //arm steady
                    //updateS2(S2mode);
                    //lcd_clear();
                    //printf("gotosort %d", S3steps);
                }
                else if (S3steps < (24+22+8)){ //bring arm in
                    S3mode = 4; //motor steady
                    //updateS3(S3mode);  
                    S2mode = 1; //arm in
                    updateS2(S2mode);
                    //lcd_clear();
                    //printf("armin %d", S3steps);
                }
                else if (S3steps < (24+22+8+3)){ //test
                    S2mode = 4;
                    S3mode = 4;
                    readADC(tab);                
                    if(ADRESH==0){tabState = 1;} //tab
                    //lcd_clear();
                    //printf("value: %x", ADRESL);
                    //lcd_newline();
                    //printf("testing %d", S3steps);
                }
                else if (S3steps < (24+22+8+3+8)){ //bring arm out
                    S2mode = 2; //arm out
                    updateS2(S2mode);
                    S3mode = 4; //motor steady
                    //updateS2(S2mode);
                    //lcd_clear();
                    //printf("armout %d", S3steps);
                }
                else if (tabState == 1){
                    
                    //lcd_clear();
                    //printf("tab");
                    //lcd_newline();
                    if(S3steps < (24+22+8+3+8+9)){ //sort                        
                        S3mode = 1;  //motor outwards
                        updateS3(S3mode);
                        S2mode = 4;  //arm steady
                        //printf("sorting %d", S3steps);
                    }
                    else if(S3steps < (24+22+8+3+8+7+(9+22))){ //return
                        S3mode = 2;  //motor inwards
                        updateS3(S3mode);
                        S2mode = 4;  //arm steady
                        //printf("returning %d", S3steps);
                    }
                    else{tabState = 100;}
                }
                else if(tabState == 0){
                    //lcd_clear();
                    //printf("no tab");
                    //lcd_newline();
                    if(S3steps < (24+22+8+3+8+(22+29))){ //sort                        
                        S3mode = 2;  //motor inwards
                        updateS3(S3mode);
                        S2mode = 4;  //arm steady
                        //printf("sorting %d", S3steps);
                    }
                    else if(S3steps < (24+22+8+3+8+(22+29)+28)){
                        S3mode = 1;  //motor outwards
                        updateS3(S3mode);
                        S2mode = 4;  //arm steady
                        //printf("returning %d", S3steps);
                    }
                    else{tabState = 100;}
                }
                else{
                    //lcd_clear();
                    //printf("finished %d", S3steps);
                    //prevSoupTime = currMom();
                    S3steps = 0;
                    tabState = 0;
                    if (timePassed(prevSodaLoad) > 2) {sodaSort = 0;}
                }
            }
            else{
                //S1shake();
                S3mode = 3; //motor shake
                updateS3(S3mode);
                S2mode = 4; //arm steady   
            }
            
        }
        
        // </editor-fold>
        else{ //sorting mode   
            
            // <editor-fold defaultstate="collapsed" desc="soup can path">
            lcd_clear();
            printf("soup can test");
            lcd_newline();
            printf("%d  %d", S1mode, soupSort);
            
            if(soupLoad){
                prevSoupLoad = currMom();
                if (!soupSort){
                    soupSort = 1;
                    //prevSoupTime = currMom();
                    S1steps = 0;
                    __delay_ms(150);
                }
            }   
            
            if (soupSort){
                //__delay_ms(10);
                S1steps ++;

                if (S1steps < 20){
                    S1mode = 3;
                    updateS1(S1mode);
                    //S1shake();
                    
                    readADC(label);                
                    if(ADRESL<=0x1){labelState = 1;} //no label 
                    lcd_clear();
                    printf("value: %x", ADRESL);
                    lcd_newline();
                    printf("testing %d", S1steps);
                }
                else if (labelState == 1){
                    //keypress = NULL;
                    //keyinterrupt();
                    //if (keypress != NULL) {//stop sorting
                    //    standby = 1;
                    //}

                    lcd_clear();
                    printf("no label");
                    lcd_newline();
                    if(S1steps < (20+30)){
                        S1mode = 1;
                        updateS1(S1mode);
                        printf("sorting %d", S1steps);
                    }
                    else if(S1steps < (20+30+29)){
                        //S1backward();
                        S1mode = 2;
                        updateS1(S1mode);
                        printf("returning %d", S1steps);
                    }
                    else{labelState = 100;}
                }
                else if(labelState == 0){
                    lcd_clear();
                    printf("label");
                    lcd_newline();
                    if(S1steps < (20+26)){
                        //S1backward();
                        S1mode = 2;
                        updateS1(S1mode);
                        printf("sorting %d", S1steps);
                    }
                    else if(S1steps < (20+26+26)){
                        //S1forward();
                        S1mode = 1;
                        updateS1(S1mode);
                        printf("returning %d", S1steps);
                    }
                    else{labelState = 100;}
                }
                else{
                    lcd_clear();
                    printf("finished %d", S1steps);
                    //prevSoupTime = currMom();
                    S1steps = 0;
                    labelState = 0;
                    if (timePassed(prevSoupLoad) > 2) {soupSort = 0;}
                }
            }
            else{
                //S1shake();
                S1mode = 3;
                updateS1(S1mode);
            }
        // </editor-fold>           
            
            // <editor-fold defaultstate="collapsed" desc="soda can path">
            readADC(IR2);            
            if(ADRES >= 0x3b5){
                prevSodaLoad = currMom();
                if (!sodaSort && (t3== 1 || t3==8)){
                    sodaSort = 1;
                    //prevSoupTime = currMom();
                    //S2steps = 0;
                    S3steps = 0;
                    //__delay_ms(300);
                }
                if(timePassed(startTime) < 1){sodaSort = 0;}
            }   
                        
            lcd_newline();
            printf("%d %x %d %x", sodaSort, ADRES, prevSodaLoad, sodaThresh);
            
            if (sodaSort){
                //__delay_ms(10);
                //S2steps ++;
                S3steps ++;

                if(S3steps < 24){
                    S3mode = 3; //motor shake
                    updateS3(S3mode);   
                    S2mode = 4; //arm steady
                }
                else if (S3steps < (24+20)){      //go to sorting position
                    S3mode = 1; //motor outwards
                    updateS3(S3mode);   
                    S2mode = 4; //arm steady
                    //updateS2(S2mode);
                    //lcd_clear();
                    //printf("gotosort %d", S3steps);
                }
                else if (S3steps < (24+20+8)){ //bring arm in
                    S3mode = 4; //motor steady
                    //updateS3(S3mode);  
                    S2mode = 1; //arm in
                    updateS2(S2mode);
                    //lcd_clear();
                    //printf("armin %d", S3steps);
                }
                else if (S3steps < (24+20+8+6)){ //test
                    S2mode = 4;
                    S3mode = 4;
                    readADC(tab);                
                    if(ADRESH==0){tabState = 1;} //tab
                    //lcd_clear();
                    //printf("value: %x", ADRESL);
                    //lcd_newline();
                    //printf("testing %d", S3steps);
                }
                else if (S3steps < (24+20+8+6+8)){ //bring arm out
                    S2mode = 2; //arm out
                    updateS2(S2mode);
                    S3mode = 4; //motor steady
                    //updateS2(S2mode);
                    //lcd_clear();
                    //printf("armout %d", S3steps);
                }
                else if (tabState == 1){
                    
                    //lcd_clear();
                    //printf("tab");
                    //lcd_newline();
                    if(S3steps < (24+20+8+6+8+9)){ //sort                        
                        S3mode = 1;  //motor outwards
                        updateS3(S3mode);
                        S2mode = 4;  //arm steady
                        //printf("sorting %d", S3steps);
                    }
                    else if(S3steps < (24+20+8+6+8+7+(9+22)-2)){ //return
                        S3mode = 2;  //motor inwards
                        updateS3(S3mode);
                        S2mode = 4;  //arm steady
                        //printf("returning %d", S3steps);
                    }
                    else{tabState = 100;}
                }
                else if(tabState == 0){
                    //lcd_clear();
                    //printf("no tab");
                    //lcd_newline();
                    if(S3steps < (24+20+8+6+8+(22+29))){ //sort                        
                        S3mode = 2;  //motor inwards
                        updateS3(S3mode);
                        S2mode = 4;  //arm steady
                        //printf("sorting %d", S3steps);
                    }
                    else if(S3steps < (24+20+8+6+8+(22+29)+29)){
                        S3mode = 1;  //motor outwards
                        updateS3(S3mode);
                        S2mode = 4;  //arm steady
                        //printf("returning %d", S3steps);
                    }
                    else{tabState = 100;}
                }
                else{
                    //lcd_clear();
                    //printf("finished %d", S3steps);
                    //prevSoupTime = currMom();
                    S3steps = 0;
                    tabState = 0;
                    if (timePassed(prevSodaLoad) > 2) {sodaSort = 0;}
                }
            }
            else{
                S3mode = 3; //motor shake
                updateS3(S3mode);
                S2mode = 4; //arm steady   
            }
            // </editor-fold>
                    
            /*
            if(standby==1){
                lcd_clear();
                printf("Finished sorting! ");
                __delay_ms(500);
                PWM2off();
                dc = 0; //OFF
                updateEEPROM(timePassed(startTime));
            }*/
        }
    }
    return;
}
