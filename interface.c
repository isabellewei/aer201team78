#include "constants.h"


void displayLogs(void){
    lcd_clear();
    printf("Run#1    A:Next");
    lcd_newline();
    printf("# of cans: 10");
    keycheck();
    lcd_clear();
    printf("Run#1    A:Next");
    lcd_newline();
    printf("# soda cans: 6");
    keycheck();
    lcd_clear();
    printf("Run#1    A:Next");
    lcd_newline();
    printf("# soup cans: 4");
    keycheck();
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
