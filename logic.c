#include "declarations.h"

int backlogTest(int blockedStart, int motorStatus){
    if (ADRESH == 0){  //unblocked
        return 1; //motor ON
    }   
    else{
        if (currMom() - blockedStart > 3){
            
            return 1; //motor ON
        }
        else{
            
            return 0;
        }
    }            
}

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



    