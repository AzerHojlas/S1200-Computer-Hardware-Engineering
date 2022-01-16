/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

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

  // TIMER
  T2CONCLR = 0xffff;  // Nollställer kontrollregister
  IFSCLR(0) = 0x100;  // Nollställer flagga

  T2CONSET = (7 << 4);  // Prescale value
  PR2 = 0x7a12;
  TMR2 = 0x0;

  T2CONSET = (1 << 15); // Set timer

  // INTERRUPT
  IPCSET(2) = 0x1f; // Högsta prioritet
  IECSET(0) = 0x100;

  enable_interrupt();

  return;
}

void labwork( void )
{
  //print_pong();
	menu();

  return;
}