#include "constants.h"
#include <stdint.h>


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