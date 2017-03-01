#include "constants.h"

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


    