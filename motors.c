#include "declarations.h"
#include <stdint.h>

int s1 = 0;
int s2 = 0;
int s3 = 0;
int s = 0;
int t = 0;
int t2 = 0;
int t3 = 0;
int prev = 0;
int curr;

void PWM1(int duty){
    // Tosc = 1/32 Mhz = 0.0000003125
    // PWM Period = (PR2+1) x 4 x (1/_XTAL_FREQ) x TMR2PRESCALE 
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

void PWM2(int duty, int dir){ //1:CW, 2:CCW
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
       
    LATAbits.LATA6 = 0;
    LATAbits.LATA7 = 0;
    __delay_ms(50);
    if(dir==1){ //CW
        LATAbits.LATA6 = 1;
        LATAbits.LATA7 = 0; 
    }
    else{       //CCW
        LATAbits.LATA6 = 0;
        LATAbits.LATA7 = 1; 
    }
        
}

void PWM2off(){
    CCP2CONbits.DC2B0 = 0;
    CCP2CONbits.DC2B1 = 0;
    CCPR2L = 0;
}

void updateS1(int mode){
    __delay_ms(5);
        
    if(mode == 0){
        S1_1 = 0;
        S1_2 = 0;
        S1_3 = 0;
        S1_4 = 0;
    }
    else if(mode == 3){
        t++;
        if (t>8) {t=1;} 
        __delay_ms(5);
        if (t == 1 || t==8){S1_1 = 1;}
        else{S1_1 = 0;} 
        if (t == 2 || t==7){S1_2 = 1;}
        else{S1_2 = 0;}
        if (t == 3 || t==6){S1_3 = 1;}
        else{S1_3 = 0;}
        if (t == 4 || t==5){S1_4 = 1;}
        else{S1_4 = 0;}          
    }
    else{
        __delay_ms(10);
        
        if(mode == 1 || mode == 5){
            s1 ++;
            if (s1>4) {s1=1;} 
            
        }
        else if(mode == 2 || mode==6){
            s1 --;
            if (s1<1) {s1=4;}            
        }
        
        if(mode == 1 || mode ==2){
            if (s1 == 1){S1_1 = 1;}
            else{S1_1 = 0;} 
            if (s1 == 2){S1_2 = 1;}
            else{S1_2 = 0;}
            if (s1 == 3){S1_3 = 1;}
            else{S1_3 = 0;}
            if (s1 == 4){S1_4 = 1;}
            else{S1_4 = 0;}   
        }
        if(mode == 5 || mode ==6){
            if (s1 == 1){S1_1 = 1; S1_2 = 1; S1_3 = 0; S1_4 = 0;}
            if (s1 == 2){S1_1 = 0; S1_2 = 1; S1_3 = 1; S1_4 = 0;}
            if (s1 == 3){S1_1 = 0; S1_2 = 0; S1_3 = 1; S1_4 = 1;}
            if (s1 == 4){S1_1 = 1; S1_2 = 0; S1_3 = 0; S1_4 = 1;}
        }
        
    }
}

void updateS2(int mode){
    __delay_ms(5);
        
    if(mode == 0){
        S2_1 = 0;
        S2_2 = 0;
        S2_3 = 0;
        S2_4 = 0;
    }
    else if(mode == 4){
        S2_1 = 1;
        S2_2 = 0;
        S2_3 = 0;
        S2_4 = 0;
    }
    else if(mode == 3){
        t2++;
        if (t2>8) {t2=1;} 
        if (t2 == 1 || t2==8){S2_1 = 1;}
        else{S2_1 = 0;} 
        if (t2 == 2 || t2==7){S2_2 = 1;}
        else{S2_2 = 0;}
        if (t2 == 3 || t2==6){S2_3 = 1;}
        else{S2_3 = 0;}
        if (t2 == 4 || t2==5){S2_4 = 1;}
        else{S2_4 = 0;}          
        }
    else{     
        if(mode == 1){
            s2 ++;
            if (s2>4) {s2=1;} 
        }
        else if(mode == 2){
           s2 --;
        if (s2<1) {s2=4;} 
        }
        if (s2 == 1){S2_1 = 1;}
        else{S2_1 = 0;} 
        if (s2 == 2){S2_2 = 1;}
        else{S2_2 = 0;}
        if (s2 == 3){S2_3 = 1;}
        else{S2_3 = 0;}
        if (s2 == 4){S2_4 = 1;}
        else{S2_4 = 0;}        
    }
}

void updateS3(int mode){
    __delay_ms(5);
        
    if(mode == 0){
        S3_1 = 0;
        S3_2 = 0;
        S3_3 = 0;
        S3_4 = 0;
    }
    else if(mode == 4){
        S3_1 = 1;
        S3_2 = 0;
        S3_3 = 0;
        S3_4 = 0;
    }
    else if(mode == 3){
            t3++;
            if (t3>8) {t3=1;} 
            if (t3 == 1 || t3==8){S3_1 = 1;}
            else{S3_1 = 0;} 
            if (t3 == 2 || t3==7){S3_2 = 1;}
            else{S3_2 = 0;}
            if (t3 == 3 || t3==6){S3_3 = 1;}
            else{S3_3 = 0;}
            if (t3 == 4 || t3==5){S3_4 = 1;}
            else{S3_4 = 0;}          
        }
    else{
        if(mode == 1){
            s3 ++;
            if (s3>4) {s3=1;} 
        }
        else if(mode == 2){
            s3 --;
            if (s3<1) {s3=4;} 
        }
        if (s3 == 1){S3_1 = 1;}
        else{S3_1 = 0;} 
        if (s3 == 2){S3_2 = 1;}
        else{S3_2 = 0;}
        if (s3 == 3){S3_3 = 1;}
        else{S3_3 = 0;}
        if (s3 == 4){S3_4 = 1;}
        else{S3_4 = 0;}        
        
    }
}
