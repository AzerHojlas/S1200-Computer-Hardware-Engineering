/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int mytime = 0x5957;

char textstring[] = "text, more text, and even more text!";

int timeoutcount = 1;

// Uppg. 3b
int prime = 1234567;

void user_isr( void ) // isr ==interrupt service routine
{
  if (timeoutcount == 10) {
    time2string(textstring, mytime);
    display_string(3, textstring);
    display_update();
    tick(&mytime);
    timeoutcount = 0;
  }
  IFSCLR(0) = 0x100;
  timeoutcount++;
}

void labinit( void )
{
  volatile int* trise = (volatile int*) 0xbf886100;
  *trise &= ~0xff;

  TRISD |= 0xfe0; // 8 bits are set to 1, i.e input

  T2CONCLR = 0xffff; // control register is cleared
  IFSCLR(0) = 0x100; // macro to set flag to 0

  T2CONSET = (7 << 4); // 111 is 7, and we jump to the position of index 4 where 256 prescaler is
  PR2 = 0x7a12;        // 
  TMR2 = 0x0;

  T2CONSET = (1 << 15);

  // Uppg. 3h
  IPCSET(2) = 0x1f; // Högsta prioritet, but we will not get any more interrupts anyway
  IECSET(0) = 0x100; // enable interrupt which is on bit index 0

  enable_interrupt(); // enable interrupt with command "ei" at enable interrupt label

  return;
}

void labwork( void )
{
  prime = nextprime(prime);
  display_string(0, itoaconv(prime));
  display_update();
}