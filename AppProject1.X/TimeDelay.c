/*
 * File:   TimeDelay.c
 * Author: Rayyan Khalil, Arda Onen
 */


#include "xc.h"
#include "TimeDelay.h"


void delay_ms(uint16_t time_ms, uint8_t idle_on)
{
    //T2CON configuration
    T2CONbits.TSIDL = 0; //operate in idle mode
    T2CONbits.T32 = 0; // operate timer 2 as 16 bit timer
    T2CONbits.TCS = 0; // use internal clock
    
    // Timer 2 interrupt config
    IPC1bits.T2IP = 2; //7 is highest and 1 is lowest priority
    IEC0bits.T2IE = 1; //enable timer interrupt
    IFS0bits.T2IF = 0; // Clear timer 2 flaf
    
    PR2 = time_ms << 4; 
    TMR2 = 0;
    T2CONbits.TON = 1; //start timer
    
    if(idle_on == 1)
    {
        Idle(); 
    }
    T2CONbits.TON=0; // Stop timer
    return;
}

// Used in HW IO control Driver project 3
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF=0; //Clear timer 2 interrupt flag
    return;
}
