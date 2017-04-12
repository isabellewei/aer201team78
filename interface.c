#include "declarations.h"

void homescreen(void){
    lcd_home();
    printf("%02x/%02x/%02x ", time[6],time[5],time[4]);    //Print date in YY/MM/DD
    printf("3:Start");
    lcd_newline();
    printf("%02x:%02x:%02x", time[2],time[1],time[0]);    //HH:MM:SS
    printf(" A:Logs");
}

void keycheck(void){
    while(PORTBbits.RB1 == 0){} //wait for keypad interrupt pin to be triggered
    keypress = (PORTB & 0xF0)>>4; // Read the 4 bit character code
    while(PORTBbits.RB1 == 1){} // Wait until the key has been release
    Nop();  //breakpoint b/c compiler optimizations
    return;
}
void keyinterrupt(void){
    if(PORTBbits.RB1 == 1){ //check status of keypad
        keypress = (PORTB & 0xF0)>>4; // Read the 4 bit character code
        while(PORTBbits.RB1 == 1){}  // Wait until the key has been released   
    }
    Nop();  //breakpoint b/c compiler optimizations
}

signed char eepromRead(signed char address){
    // Set address registers
    EEADRH = (signed char)(address >> 8);
    EEADR = (signed char)address;

    EECON1bits.EEPGD = 0;       // Select EEPROM Data Memory
    EECON1bits.CFGS = 0;        // Access flash/EEPROM NOT config. registers
    EECON1bits.RD = 1;          // Start a read cycle

    // A read should only take one cycle, and then the hardware will clear
    // the RD bit
    while(EECON1bits.RD == 1);

    return EEDATA;              // Return data
}

void eepromWrite(signed char address, signed char data){    
    // Set address registers
    EEADRH = (signed char)(address >> 8);
    EEADR = (signed char)address;

    EEDATA = data;          // Write data we want to write to SFR
    EECON1bits.EEPGD = 0;   // Select EEPROM data memory
    EECON1bits.CFGS = 0;    // Access flash/EEPROM NOT config. registers
    EECON1bits.WREN = 1;    // Enable writing of EEPROM (this is disabled again after the write completes)

    // The next three lines of code perform the required operations to
    // initiate a EEPROM write
    EECON2 = 0x55;          // Part of required sequence for write to internal EEPROM
    EECON2 = 0xAA;          // Part of required sequence for write to internal EEPROM
    EECON1bits.WR = 1;      // Part of required sequence for write to internal EEPROM

    // Loop until write operation is complete
    while(PIR2bits.EEIF == 0)
    {
        continue;   // Do nothing, are just waiting
    }

    PIR2bits.EEIF = 0;      //Clearing EEIF bit (this MUST be cleared in software after each write)
    EECON1bits.WREN = 0;    // Disable write (for safety, it is re-enabled next time a EEPROM write is performed)
}

void addRun(int dur, int year, int month, int date, int hours, int mins, int secs, int sodaT, int sodaNT, int soupL, int soupNL){
    int currRun = eepromRead(0)-1;
    if (currRun < 1){currRun = 4;}
    eepromWrite(0, currRun);
    
    eepromWrite(currRun*0x10 + 1, dur);
    eepromWrite(currRun*0x10 + 2, year);
    eepromWrite(currRun*0x10 + 3, month);
    eepromWrite(currRun*0x10 + 4, date);
    eepromWrite(currRun*0x10 + 5, hours);
    eepromWrite(currRun*0x10 + 6, mins);
    eepromWrite(currRun*0x10 + 7, secs);
    eepromWrite(currRun*0x10 + 8, sodaT);
    eepromWrite(currRun*0x10 + 9, sodaNT);
    eepromWrite(currRun*0x10 + 0xa, soupL);
    eepromWrite(currRun*0x10 + 0xb, soupNL);
}

void displayLog(int run){
    int currRun = eepromRead(0);
    int i;
    for (i=1;i<(run);i++){
        currRun++;
        if (currRun > 4){currRun = 1;}
    }
        
    lcd_clear();
    printf("Run#%d    A:Next", run);
    lcd_newline();
    printf("Runtime: %d", eepromRead(currRun*0x10 + 1));
    keycheck();    
    lcd_clear();
    printf("Run#%d    A:Next", run);
    lcd_newline();
    printf("Date: %02x/%02x/%02x", eepromRead(currRun*0x10 + 2),eepromRead(currRun*0x10 + 3),eepromRead(currRun*0x10 + 4));
    keycheck();    
    lcd_clear();
    printf("Run#%d    A:Next", run);
    lcd_newline();
    printf("Start: %02x:%02x:%02x", eepromRead(currRun*0x10 + 5), eepromRead(currRun*0x10 + 6), eepromRead(currRun*0x10 + 7));
    keycheck();        
    lcd_clear();
    printf("Run#%d    A:Next", run);
    lcd_newline();
    printf("Soda w/ tab: %d", eepromRead(currRun*0x10 + 8));
    keycheck();
    lcd_clear();
    printf("Run#%d    A:Next", run);
    lcd_newline();
    printf("Soda w/o tab: %d", eepromRead(currRun*0x10 + 9));
    keycheck();
     lcd_clear();
    printf("Run#%d    A:Next", run);
    lcd_newline();
    printf("Soup w/ lbl: %d", eepromRead(currRun*0x10 + 0xa));
    keycheck();
     lcd_clear();
    printf("Run#%d    A:Next", run);
    lcd_newline();
    printf("Soup w/o lbl: %d", eepromRead(currRun*0x10 + 0xb));
    keycheck();
}