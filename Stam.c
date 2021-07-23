
#include <xc.h>
#include <sys/attribs.h>
#include "config.h"
#include "Stam.h"



void __ISR(_TIMER_1_VECTOR, ipl7auto) Timer1ISR(void) 
{  
    globaltime+=STAM_TMR_TIME*10000;
    IFS0bits.T5IF = 0;     // clear interrupt flag
}




void SSD_Timer1Setup()
{
  PR1 = (int)(((float)(STAM_TMR_TIME * PB_FRQ) / 256) + 0.5); //set period register, generates one interrupt every 3 ms
  TMR1 = 0;                           //    initialize count to 0
  T1CONbits.TCKPS = 2;                //    1:64 prescale value
  T1CONbits.TGATE = 0;                //    not gated input (the default)
  T1CONbits.TCS = 0;                  //    PCBLK input (the default)
  T1CONbits.ON = 1;                   //    turn on Timer1
  IPC1bits.T1IP = 7;                  //    priority
  IPC1bits.T1IS = 3;                  //    subpriority
  IFS0bits.T1IF = 0;                  //    clear interrupt flag
  IEC0bits.T1IE = 1;                  //    enable interrupt
  macro_enable_interrupts();          //    enable interrupts at CPU
}

 