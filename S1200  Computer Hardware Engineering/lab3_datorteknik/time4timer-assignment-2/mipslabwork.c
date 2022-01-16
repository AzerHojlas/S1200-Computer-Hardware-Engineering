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

// Uppg. 2c
int timeoutcount = 1;

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

void labinit( void )
{
  volatile int* trise = (volatile int*) 0xbf886100;
  *trise &= ~0xff;

  TRISD |= 0xfe0;

  // Uppg. 2b
  T2CONCLR = 0xffff;  // Nollställer kontrollregister
  IFSCLR(0) = 0x100;  // Nollställer flagga

  T2CONSET = (7 << 4);  // 111 = 1:256 prescale value
  PR2 = 0x7a12; // Control register is set to 31 250, after we get a timeout
  TMR2 = 0x0;   // 8 MHz is predefined oscillator of timer 2, we used a prescaler of 256 to be able to get 312 500 to a small enough size to divide by 10

  T2CONSET = (1 << 15);  // Set timer

  return;
}

void labwork( void )
{
  volatile int* porte = (volatile int*) 0xbf886110;

  int button = getbtns();
  int switches = getsw();

  if (button & 4) {
    mytime &= 0xffff0fff;
    mytime |= switches << 12;
  }

  if (button & 2) {
    mytime &= 0xfffff0ff;
    mytime |= switches << 8;
  }

  if (button & 1) {
    mytime &= 0xffffff0f;
    mytime |= switches << 4;
  }

  // Uppg. 2b
  if (IFS(0) & 0x100) { // Om bit 8 är lika med 1 i flaggregistret
    if (timeoutcount == 10) { // Uppg. 2c När flaggan har satts till 1 tio gånger kommer detta block att initieras
      time2string(textstring, mytime);
      display_string(3, textstring);
      display_update();
      tick(&mytime);
      display_image(96, icon);
      (*porte)++;
      timeoutcount = 1;
    }
    IFSCLR(0) = 0x100;  // Nollställer flaggan
    timeoutcount++;     // Ökar antalet timeouts vi haft
  }
}