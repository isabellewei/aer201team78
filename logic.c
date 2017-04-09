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

    