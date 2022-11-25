/*
 * File:   main.c
 * Author: Rayyan Khalil, Arda Onen
 *
 * Created on September 26, 2020, 9:17 PM
 */

// MPLAB header libraries
#include <xc.h>

#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

// User header files

#include "ChangeClk.h"
#include "TimeDelay.h"
#include "IOs.h"
#include "UART2.h"

#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled

// Global variables
uint8_t CNflag = 0;
uint8_t minutes = 0;
uint8_t seconds = 0;
int countdown_start = 0;
int countdown_pause = 0;
int countdown_reset = 0;
int alarm = 0;

// MACROS for Idle, Sleep modes
#define Nop() {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}
#define Sleep() {__asm__ volatile ("pwrsav #0");}   //Sleep() - put MCU in sleep mode - CPU and some peripherals off
#define Idle() {__asm__ volatile ("pwrsav #1");}    //Idle() - put MCU in idle mode - only CPU off
#define dsen() {__asm__ volatile ("BSET DSCON, #15");} //

//MAIN
void main(void) {
   
    // enables IO and CN interrupts on Push buttons
    IOinit(); 
    // initializes UART2
    InitUART2();
    
    // Display initial timer at 00m : 00s
    NewClk(8);
    
    DispTime(minutes, seconds);
    
    NewClk(32);
    
    while(1)
    {
        Idle();    
    }
    
    return;
}
