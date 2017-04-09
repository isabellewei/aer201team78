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
int seconds = 0;
int S1mode = 0;
int S2mode = 0;
int S3mode = 0;
int soupSteps;
int sodaSteps;

void interrupt handler(void) {   
    //** 1 SECOND TIMER THAT CALLS printRTC() using Timer0 **
    di();
    if(TMR0IF){
        TMR0IF = 0;
        seconds++;
        TMR0ON = 0;
    
        // Initialize timer again!
        T0CON = 0b00010111;
        TMR0H = 0b10000101;
        TMR0L = 0b10101110;
        T0CON = T0CON | 0b10000000;
    }
    
    if(TMR1IF){
        TMR1IF = 0;
        updateS1(S1mode);
        updateS2(S2mode);
        updateS3(S3mode);   
        soupSteps ++;
        sodaSteps ++;
        TMR1ON = 0;
               
        // Initialize timer again!
        T1CON = 0b10110000;
        TMR1H = 0b11101001;
        TMR1L = 0b01011011;
        TMR1ON = 1; 
    }  
}

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

    I2C_Master_Init(32000); //Initialize I2C Master with 100KHz clock
    
    //initialize ADC
    nRBPU = 0;
    //ADCON0 = 00000001;  //Enable ADC
    ADCON1 = 0b00001001;  //AN0 to AN5 used as analog input
    ADCON2 = 0b10001001;  //right justified, 
    CVRCON = 0x00; // Disable CCP reference voltage output
    CMCONbits.CIS = 0;
    ADFM = 1;
    
    TMR0IE = 1; // Enable Timer0 interrupts
    TMR1IE = 1; // Enable Timer1 interrupts
    //T0CON = 0b11011000; //ON, 8-bit, internal clock, rising edge, prescale, 010 prescale value
    PEIE = 1; // Set peripheral interrupt enable bit (YOU MUST DO THIS IF YOU
              // DIDN'T SET INTERRUPT PRIORITIES (i.e. if IPEN=0)
    ei(); // Enable all interrupts for now
}

int main(void) {
    LATA = 0;
    // <editor-fold defaultstate="collapsed" desc="initialization">
    initialize();   
    int standby = 1; //1=true, 0=false
    int s = 0;      //stepper motor counter
    int prevSodaIR;
    int prevSoupIR;
    long startTime;
    
    int soupLoad = 1; //can waiting to be sorted?
    int sodaLoad = 1; //can waiting to be sorted?
       
    int tabState = 0; //0:no, 1: yes
    int labelState = 0; //current can has label or not?
    
    int soupSort; //sort soup  cans?
    int prevSoupLoad; //most recent soup can pass
    int sodaSort; //sort soup  cans?
    int prevSodaLoad; //most recent soup can pass
    
    int soupL;
    int soupNL ;
    int sodaT;
    int sodaNT;
    
    T0CON = 0b00010111;
    TMR0H = 0b10000101;
    TMR0L = 0b10101110;
    T0CONbits.TMR0ON = 1;
    T1CON = 0b10110000;
    TMR1H = 0b11111101;
    TMR1L = 0b01011011;
    TMR1ON = 1; 
    // </editor-fold>
    
    int ee = 0x100;
            
    __delay_ms(10);       
            
    while(1){
        //34updateTime();                
        if (standby == 1){            
            keypress = NULL;
            while(keypress == NULL){
                updateTime();
                homescreen();
                keyinterrupt();
            }
                     
                                 
            if(keypress == 2){ //3: Start sorting
                // <editor-fold defaultstate="collapsed" desc="sort intitialize">
                standby = 0; //not standby
                startTime = currMom();             
                seconds = 0;  
                labelState = 0;
                soupSort = 0;                
                tabState = 0;
                sodaSort = 0;
                soupSteps = 0;
                sodaSteps = 0; 
                soupL = 0;
                soupNL = 0;
                sodaT = 0;
                sodaNT = 0;
                prevSoupIR = 0;
                prevSodaIR = 0;
                
                keypress = NULL;  
                lcd_clear();
                printf("Sorting cans...");
                lcd_newline();
                printf("Any key to stop");
                                
                while(soupSteps < 4 ){
                    S2mode = 2; //arm out
                }                
                S2mode = 4;
                int i;
                for(i=0; i<70; i++){
                    updateS1(1);
                    readADC(IR1);
                    if(ADRES <prevSodaIR && ADRES > wheelThresh){break;}
                    prevSodaIR = ADRES;
                }
                for(i=0; i<100; i++){
                    updateS1(2);
                    readADC(IR1);
                    if(ADRES <prevSodaIR && ADRES > wheelThresh){break;}
                    prevSodaIR = ADRES;
                }
                for(i=0; i<70; i++){
                    updateS3(1);
                    readADC(IR4);
                    if(ADRES <prevSodaIR && ADRES > wheelThresh){break;}
                    prevSodaIR = ADRES;
                }
                for(i=0; i<100; i++){
                    updateS3(2);
                    readADC(IR4);
                    if(ADRES <prevSodaIR && ADRES > wheelThresh){break;}
                    prevSodaIR = ADRES;
                }
                PWM2(400);
                PWM1(400);
                // </editor-fold>                        
            }
            else if(keypress == 3){ //2:Logs
                displayLog(1);
                displayLog(2);
                displayLog(3);
                displayLog(4);
            }
            else if(keypress == 15 ) { //D: testing components
                standby = 2;   
                s = 1;
            }
            
        }
        else if(standby == 2){ //testing components
            // <editor-fold defaultstate="collapsed" desc="Testing Components">
            keypress = NULL; 
            
            lcd_clear();
            printf("testing S f");
            S1mode = 1;
            S2mode = 1;
            S3mode = 1;              
            keycheck();

            lcd_clear();
            printf("testing S b");                
            S1mode = 2;
            S2mode = 2;
            S3mode = 2;                            
            keycheck();
            
            lcd_clear();
            printf("testing S shake");                
            S1mode = 3;
            S2mode = 3;
            S3mode = 3;
            keycheck();
                        
            S1mode = 0;
            S2mode = 0;
            S3mode = 0;
            
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
                
                readADC(IR1);
                lcd_newline();
                printf("%x", ADRES);
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
                
                readADC(IR4);
                lcd_newline();
                printf("%x", ADRES);
            }

            lcd_clear();
            printf("testing PWM2");
            PWM2(400);
            keycheck();
            PWM2off();

            lcd_clear();
            printf("testing PWM1");
            PWM1(400);
            keycheck();
            PWM1off();

            keypress = NULL;
            seconds = 0;              
            while(keypress==NULL){
                keypress = NULL;
                //__delay_ms(50);
                lcd_clear();
                printf("testing TMR0");
                lcd_newline();
                printf("%d", seconds);
                keyinterrupt();
            }

            keypress = NULL;
            while(keypress==NULL){
                keypress = NULL;
                //__delay_ms(50);
                lcd_clear();
                printf("testing IR");
                lcd_newline();
                readADC(IR1);
                printf("%x ", ADRES);
                readADC(IR2);
                printf("%x ", ADRES);
                readADC(IR3);
                printf("%x ", ADRES);
                readADC(IR4);
                printf("%x ", ADRES);
                keyinterrupt();
                __delay_ms(100);
            }

            keypress = NULL;
            while(keypress==NULL){
                keypress = NULL;
                __delay_ms(50);
                lcd_clear();
                printf("label   tab");
                lcd_newline();
                readADC(label);
                printf("%x      ", ADRES);
                readADC(tab);
                printf("%x      ", ADRES);
                keyinterrupt();
            }

            standby = 1;
                                 
            /*if(keypress != NULL){
                s++;
            }*/
        // </editor-fold>    
        } 
        else{ //sorting mode           
            lcd_clear();
            // <editor-fold defaultstate="collapsed" desc="soup can path">
            readADC(IR3);
            if(ADRES <= canThresh){
                prevSoupLoad = seconds;
                if (!soupSort){
                    soupSort = 1;
                    soupSteps = 0;
                    __delay_ms(150);
                }
            }   
            
            printf("%x %d", ADRES, soupSteps);
            
            if (soupSort){    
                if (soupSteps < 80){
                    S1mode = 3; //shake
                    readADC(label);                
                    if(ADRES<0x200){labelState = 1;} //no label 
                    printf(" %x",ADRES);
                }
                else if (labelState == 1){ //no label
                    if(soupSteps < (80+70)){
                        S1mode = 1;
                        prevSoupLoad = 0;
                    }
                    else if(soupSteps < (80+70+60)){
                        S1mode = 2;                        
                    }
                    else{
                        S1mode = 0;
                        updateS1(2);
                        readADC(IR1);                        
                        printf(" %x", ADRES);
                        if (ADRES < prevSoupIR && ADRES > wheelThresh){labelState = 101;}
                        prevSoupIR = ADRES;
                        
                    }
                }
                else if(labelState == 0){ //label
                    if(soupSteps < (80+70)){
                        S1mode = 2;     
                        prevSoupLoad = 0;
                    }
                    else if(soupSteps < (80+70+60)){
                        S1mode = 1;                       
                    }
                    else{
                        S1mode = 0;
                        updateS1(1);
                        readADC(IR1);                       
                        printf(" %x", ADRES);
                        if(ADRES < prevSoupIR && ADRES > wheelThresh){labelState = 100;}
                        prevSoupIR = ADRES;
                    }
                }
                else{
                    if(labelState == 101){soupNL++;}
                    else if(labelState == 100){soupL++;}
                    soupSteps = 0;
                    prevSoupIR = 0;
                    labelState = 0;
                    if (prevSoupLoad == 0) {soupSort = 0;}
                }
            }
            else{
                S1mode = 3; //shake
            }
        // </editor-fold>           
            
            lcd_newline();
            // <editor-fold defaultstate="collapsed" desc="soda can path">
            readADC(IR2);            
            if(ADRES <= canThresh){
                prevSodaLoad = seconds;
                if (!sodaSort){
                    sodaSort = 1;
                    sodaSteps = 0;                    
                    __delay_ms(150);
                }
            }                          
            
            printf("%x %d", ADRES, sodaSteps);
            
            if (sodaSort){
                if(sodaSteps < 48){
                    S3mode = 3; //motor shake
                    S2mode = 4; //arm steady
                }
                else if (sodaSteps < (48+44)){      //go to sorting position
                    S3mode = 1; //motor outwards
                    S2mode = 4; //arm steady
                }
                else if (sodaSteps < (48+44+16)){ //bring arm in
                    S3mode = 4; //motor steady  
                    S2mode = 1; //arm in
                }
                else if (sodaSteps < (48+44+16+12)){ //test
                    S2mode = 4;
                    S3mode = 4;
                    readADC(tab);                
                    if(ADRESH<=1){tabState = 1;} //tab
                    printf("%x",ADRESH);
                }
                else if (sodaSteps < (48+44+16+12+16)){ //bring arm out
                    S2mode = 2; //arm out
                    S3mode = 4; //motor steady
                }
                else if (tabState == 1){ //tab
                    if(sodaSteps < (48+44+16+12+16+30)){ //sort                        
                        S3mode = 1;  //motor outwards
                        S2mode = 4;  //arm steady
                        prevSodaLoad = 0;                        
                    }
                    else if(sodaSteps < (48+44+16+12+16+30+57)){ //return                    
                        S3mode = 2;  //motor inwards
                        S2mode = 4;  //arm steady
                    }
                    else { //return
                        S2mode = 4;  //arm steady
                        S3mode = 0;  
                        updateS3(2);      
                        readADC(IR4);
                        if(ADRES <prevSodaIR && ADRES > wheelThresh){tabState = 101;}
                        prevSodaIR = ADRES;
                        printf(" %x", ADRES);
                    }
                }
                else if(tabState == 0){ //no tab
                    if(sodaSteps < (48+44+16+12+16+(44+90))){ //sort                        
                        S3mode = 2;  //motor inwards
                        S2mode = 4;  //arm steady
                        prevSodaLoad = 0;
                    }
                    else if(sodaSteps < (48+44+16+12+16+(44+80)+65)){ //sort                        
                        S3mode = 1;  //motor outwards
                        S2mode = 4;  //arm steady
                    }
                    else {
                        S3mode = 0;  
                        S2mode = 4;  //arm steady
                        updateS3(1);
                        readADC(IR4);
                        if(ADRES <prevSodaIR && ADRES > wheelThresh){tabState = 100;}
                        prevSodaIR = ADRES;
                        printf(" %x", ADRES);
                    }
                }
                else{
                    if(tabState==101){sodaT++;}
                    else if(tabState==10){sodaNT++;}
                    sodaSteps = 0;
                    tabState = 0;
                    prevSodaIR = 0;
                    if (prevSodaLoad == 0) {sodaSort = 0;}
                }
            }
            else{
                S3mode = 3; //motor shake
                S2mode = 4; //arm steady   
            }
            // </editor-fold>
            
            keyinterrupt();
            if (/*((seconds -prevSoupLoad) > 10 && (seconds-prevSodaLoad) > 10)||*/keypress!=NULL){   
                standby = 1;   
                PWM2off();
                PWM1off();
                S1mode = 0;
                S2mode = 0;
                S3mode = 0;    
                lcd_clear();
                printf("%x", ee);
                keycheck();
               
                addRun(seconds, time[6], time[5], time[4], time[2], time[1], time[0], sodaT, sodaNT, soupL, soupNL);
           
                lcd_clear();
                printf("Sort done! A:No");
                lcd_newline();
                printf("View Logs? B:Yes");  
                keycheck();
                if(keypress == 7){
                    lcd_clear();
                    printf("%ld %d %d", startTime, seconds, eepromRead(0));
                    keycheck();
                    displayLog(1);
                }        
                                             
            }
        }
    }
    return;
}
