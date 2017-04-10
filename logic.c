#include "declarations.h"


int timePassed(int start){
    if(currMom() >= 86400){return currMom() + 86400 - start;}
    else{return currMom() - start;}
}

void readADC(char channel){
    // Select A2D channel to read
    ADCON0 = ((channel <<2));
    ADCON0bits.ADON = 1;
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO_NOT_DONE){__delay_ms(5);} 
}

void startTMR3(void){
    T3CON = 0b10110000;
     TMR3H = 0b10011001;
    TMR3L = 0b01011011;
    TMR3ON = 1; 
}

void startTMR1(void){
    T1CON = 0b10110000;
    TMR1H = 0b11101001;
    TMR1L = 0b01011011;
    TMR1ON = 1; 
}

void calibrateWheels(void){
    TMR1ON = 0;
    int i = 1;
    int prev;
    do{
        if (i < 20){updateS1(1);}
        else if(i<(20+40)){updateS1(2);}
        else if(i<(20+40+80)){updateS1(1);}
        else if(i<(20+40+80+100)){updateS1(2);}
        else{i=1;}
        
        i++;
        if (i==1){prev = 0;}
        else{prev = ADRES;}
        readADC(IR1);
    }while(ADRES>prev || ADRES<wheelThresh);
    
    do{
        if (i < 20){updateS3(1);}
        else if(i<(20+40)){updateS3(2);}
        else if(i<(20+40+80)){updateS3(1);}
        else if(i<(20+40+80+100)){updateS3(2);}
        else{i=1;}
        
        i++;
        if (i==1){prev = 0;}
        else{prev = ADRES;}
        readADC(IR4);
    }while(ADRES>prev || ADRES<wheelThresh);       
    startTMR1();     
}
    