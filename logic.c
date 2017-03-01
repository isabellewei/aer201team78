#include "constants.h"

int backlogTest(int blockedStart, int motorStatus){
    if (ADRESH == 0){  //unblocked
        PWM1(100);   
        return 1; //motor ON
    }   
    else{
        if (currMom() - blockedStart > 3){
            PWM1(100);   
            return 1; //motor ON
        }
    }            
}

int timePassed(int start){
    if(currMom() >= 86400){return currMom() + 86400 - start;}
    else{return currMom() - start;}
}


    