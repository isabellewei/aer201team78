#include "constants.h"
#include <stdint.h>

int s = 0;
int prev = 0;
int curr;

void PWM1(int duty){
    // Tosc = 1/32 Mhz = 0.0000003125
    // PWM Period = 256 x 4 x (1/_XTAL_FREQ) x TMR2PRESCALE 
    // PWM Frequency = 1/PWM Period
    // duty = duty% * (PWM period) / (1/_XTAL_FREQ)/ TMR2PRESCALE
    //       = duty% * (256 x 4 x (1/_XTAL_FREQ) x TMR2PRESCALE ) / (1/_XTAL_FREQ)/ TMR2PRESCALE
    //       = duty% * 1024
    //duty = duty% * 1024;
    CCP1CONbits.DC1B0 = duty & 1; //set low bit
    CCP1CONbits.DC1B1 = (duty >> 1) & 1;  //set second lowest
    CCPR1L = (duty >> 2); //set highest eight
        
}

void PWM1off(){
    CCP1CONbits.DC1B0 = 0;
    CCP1CONbits.DC1B1 = 0;
    CCPR1L = 0;
}

void PWM2(int duty){
    // Tosc = 1/32 Mhz = 0.0000003125
    // PWM Period = 256 x 4 x (1/_XTAL_FREQ) x TMR2PRESCALE 
    // PWM Frequency = 1/PWM Period
    // duty = duty% * (PWM period) / (1/_XTAL_FREQ)/ TMR2PRESCALE
    //       = duty% * (256 x 4 x (1/_XTAL_FREQ) x TMR2PRESCALE ) / (1/_XTAL_FREQ)/ TMR2PRESCALE
    //       = duty% * 1024
    //duty = duty% * 1024;
    CCP2CONbits.DC2B0 = duty & 1; //set low bit
    CCP2CONbits.DC2B1 = (duty >> 1) & 1;  //set second lowest
    CCPR2L = (duty >> 2); //set highest eight
        
}

void PWM2off(){
    CCP2CONbits.DC2B0 = 0;
    CCP2CONbits.DC2B1 = 0;
    //CCP2CON = 0;
    CCPR2L = 0;
}

void updateStepperN(){
    if (TMR0L>prev) {curr = TMR0L-prev;}
    else {curr = TMR0L-prev + 255;}

    if (curr > step) {
        s++;
        curr = 0;
        prev = TMR0L;
    }
    if (s>4) {s=1;} 
}

void updateStepperM(){
    curr++;
        
    if (curr > step) {
        s++;
        curr = 0;
        
    }
    if (s>4) {s=1;} 
}

void updateStepper(){
    s++;
    if (s>4) {s=1;} 
}

void S2forward(void){
    if (s == 1){S2_1 = 1;}
    else{S2_1 = 0;} 
    if (s == 2){S2_2 = 1;}
    else{S2_2 = 0;}
    if (s == 3){S2_3 = 1;}
    else{S2_3 = 0;}
    if (s == 4){S2_4 = 1;}
    else{S2_4 = 0;}
}

void S2backward(void){
    if (s == 4){S2_1 = 1;}
    else{S2_1 = 0;} 
    if (s == 3){S2_2 = 1;}
    else{S2_2 = 0;}
    if (s == 2){S2_3 = 1;}
    else{S2_3 = 0;}
    if (s == 1){S2_4 = 1;}
    else{S2_4 = 0;}
}
void S2off(void){
    if (s == 4){S2_1 = 1;}
    else{S2_1 = 0;} 
    if (s == 2){S2_2 = 1;}
    else{S2_2 = 0;}
    if (s == 3){S2_3 = 1;}
    else{S2_3 = 0;}
    if (s == 1){S2_4 = 1;}
    else{S2_4 = 0;}
}

void S1forward(void){
    if (s == 1){S1_1 = 1;}
    else{S1_1 = 0;} 
    if (s == 2){S1_2 = 1;}
    else{S1_2 = 0;}
    if (s == 3){S1_3 = 1;}
    else{S1_3 = 0;}
    if (s == 4){S1_4 = 1;}
    else{S1_4 = 0;}
}

void S1backward(void){
    if (s == 4){S1_1 = 1;}
    else{S1_1 = 0;} 
    if (s == 3){S1_2 = 1;}
    else{S1_2 = 0;}
    if (s == 2){S1_3 = 1;}
    else{S1_3 = 0;}
    if (s == 1){S1_4 = 1;}
    else{S1_4 = 0;}
}
void S1off(void){
    if (s == 4){S1_1 = 1;}
    else{S1_1 = 0;} 
    if (s == 2){S1_2 = 1;}
    else{S1_2 = 0;}
    if (s == 3){S1_3 = 1;}
    else{S1_3 = 0;}
    if (s == 1){S1_4 = 1;}
    else{S1_4 = 0;}
}

void S3forward(void){
    if (s == 1){S3_1 = 1;}
    else{S3_1 = 0;} 
    if (s == 2){S3_2 = 1;}
    else{S3_2 = 0;}
    if (s == 3){S3_3 = 1;}
    else{S3_3 = 0;}
    if (s == 4){S3_4 = 1;}
    else{S3_4 = 0;}
}

void S3backward(void){
    if (s == 4){S3_1 = 1;}
    else{S3_1 = 0;} 
    if (s == 3){S3_2 = 1;}
    else{S3_2 = 0;}
    if (s == 2){S3_3 = 1;}
    else{S3_3 = 0;}
    if (s == 1){S3_4 = 1;}
    else{S3_4 = 0;}
}
void S3off(void){
    if (s == 4){S3_1 = 1;}
    else{S3_1 = 0;} 
    if (s == 2){S3_2 = 1;}
    else{S3_2 = 0;}
    if (s == 3){S3_3 = 1;}
    else{S3_3 = 0;}
    if (s == 1){S3_4 = 1;}
    else{S3_4 = 0;}
}

