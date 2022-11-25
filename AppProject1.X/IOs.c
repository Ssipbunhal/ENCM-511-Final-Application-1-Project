/*
 * File:   IOs.c
 * Author: Rayyan Khalil, Arda Onen
 *
 * Created on November 20, 2020, 6:00 PM
 */

#include "xc.h"
#include "ChangeClk.h"
#include "TimeDelay.h"
#include "IOs.h"
#include "UART2.h"

// IOinit() with IO interrupts
void IOinit(void)
{
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital
     
    TRISBbits.TRISB8 = 0; // Make GPIO RB8 as a digital output
    LATBbits.LATB8 = 0; // Make GPIO RB8 as a digital output
    
    TRISAbits.TRISA4 = 1; // Makes GPIO RA4 as a digital input
    CNPU1bits.CN0PUE = 1; // Enables pull up resistor on RA4/CN0
    CNEN1bits.CN0IE = 1;  // enable CN on CN0
    
    TRISBbits.TRISB4 = 1; // Makes GPIO RB4 as a digital input
    CNPU1bits.CN1PUE = 1; // Enables pull up resistor on RA4/CN0
    CNEN1bits.CN1IE = 1;  // enable CN on CN1
    
    TRISAbits.TRISA2 = 1; // Makes GPIO RA2 as a digital input
    CNPU2bits.CN30PUE = 1; // Enables pull up resistor on RA4/CN0
    CNEN2bits.CN30IE = 1;  // enable CN on CN30
    
    //Interrupt settings
    IPC4bits.CNIP = 6; //7 is highest priority, 0 is disabled interrupt
    IFS1bits.CNIF = 0; // Clear interrupt flag
    IEC1bits.CNIE = 1; //Enable CN interrupts
    
    return;
}


void IOcheck(void)
{
    IEC1bits.CNIE = 0; //disable CN interrupts to avoid debounces
    delay_ms(400,1);   // 400 msec delay to filter out debounces 
    IEC1bits.CNIE = 1; //Enable CN interrupts to detect pb release
    
    int i = -1;
    
    //While only RA2 push button 1 is pressed
    while((PORTAbits.RA2 == 0) && (PORTAbits.RA4 == 1) && (PORTBbits.RB4 == 1)) 
    {
        
        minutes++; // increment minutes
        
        if(minutes > 59){ // if minutes exceed 59, reset minutes to 0
            minutes = 0;
        }
        
        alarm = 0; // clear alarm messages
        
        NewClk(8);
        DispTime(minutes, seconds); // Display updated timer
        NewClk(32); 
        
        delay_ms(400, 1); //delay of 400 ms to give the user time to stop  at desired minute
    }
    
    //While only RA4 push button 2 is pressed
    while((PORTAbits.RA2 == 1) && (PORTAbits.RA4 == 0) && (PORTBbits.RB4 == 1)) 
    {
        
        seconds++; // increment seconds
        
        if(seconds > 59){ // if seconds exceed 59 reset seconds to 0
            seconds = 0;
        }   
        
        alarm = 0; // clear alarm messages if there is any
        
        NewClk(8);
        DispTime(minutes, seconds); // display updated timer
        NewClk(32);
        
        delay_ms(400, 1); // delay of 400 ms to give user time to stop at desired second
    } 
    
    //While only RB4 push button 3 is pressed
    while((PORTAbits.RA2 == 1) && (PORTAbits.RA4 == 1) && (PORTBbits.RB4 == 0))
    {   
        i++; // increment variable i to count how many times delay of 100 ms happened
        
        delay_ms(100,1); // delay 100 ms
    }
    
    if(i > -1 && i <= 30){ // if i changed but didnt exceed 30, so push button 3 was pressed for 3 or less seconds
        
        if(countdown_start == 1){ // if state countdown_start was on, the short press will pause the countdown
            countdown_start = 0;
            countdown_pause = 1;
            countdown_reset = 0;
        }
        else{ // start the countdown by turning countdown_start on
            countdown_start = 1;
            countdown_pause = 0;
            countdown_reset = 0;
        }
    }
    
    if(i > -1 && i > 30){ //if i changed and bigger than 30, push button 3 was pressed for more than 3 seconds
        countdown_reset = 1; // reset the countdown timer
        countdown_start = 0;
        countdown_pause = 0;
    }
    
    
    while(countdown_start == 1) // while countdown timer is on
    {
        NewClk(8);
        DispTime(minutes, seconds); // display current timer time
        NewClk(32);
        
        // blink the LED at 1 sec intervals
        LATBbits.LATB8 = 1;
        delay_ms(500, 1);
        LATBbits.LATB8 = 0;
        delay_ms(500, 1);
         
        if(seconds > 0){ // if seconds > 0, decrement
            seconds--;
        }
        else if(seconds == 0 && minutes > 0){ // if seconds = 0 and minutes still > 0, set seconds to 59 and decrement minute
            seconds = 59;
            minutes--;
        }
        else{ // else if seconds = 0 and minutes = 0, stop timer turn LED on and set alarm on
            countdown_start = 0;
            LATBbits.LATB8 = 1;
            alarm = 1;
            NewClk(8);
            DispTime(minutes, seconds); // Display timer with alarm message
            NewClk(32);
        }  
    }
    
    
    
    while(countdown_pause == 1) // while the state coutdown_pause is on
    {
        NewClk(8);
        DispTime(minutes, seconds); // display the current time the timer was paused at
        NewClk(32);
        
        LATBbits.LATB8 = 1; // keep blinking the LED at 1 second intervals
        delay_ms(500, 1);
        LATBbits.LATB8 = 0;
        delay_ms(500, 1);    
    }
    
    while(countdown_reset == 1) // while state countdown_reset is on
    {
        alarm = 0; // clear alarm if exists
        minutes = 0; // set minutes to 0
        seconds = 0; // set seconds to 0
        LATBbits.LATB8 = 0; // turn LED off
        
        NewClk(8);
        DispTime(minutes, seconds); // Display reset timer
        NewClk(32);
        Idle(); // stay in Idle until another interrupt happens
    }
    
    return;
}


// Change of pin Interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
	countdown_reset = 0; // turn off state countdown_reset if exists
	IFS1bits.CNIF = 0;	// clear IF flag
    T2CONbits.TON = 0; // Disable timer
    IEC0bits.T2IE = 0; //Disable timer interrupt
    IOcheck(); 
	Nop();	
    
    return;
}
