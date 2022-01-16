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

/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

void labinit( void )
{
  // Uppg. 1c
  volatile int* trise = (volatile int*) 0xbf886100; // Variabeln trise pekar på registret TRISE
  *trise &= ~0xff;  // Sätter bit 0 till och med bit 7 till 0 (output)

  // Uppg. 1e
  TRISD |= 0xfe0; // Sätter bit 5 till och med 11 till 1 (input)

  return;
}

void labwork( void )
{
  // Uppg. 1d
  volatile int* porte = (volatile int*) 0xbf886110; // Variabeln porte pekar på registret PORTE

  // Uppg. 1h
  int button = getbtns();
  int switches = getsw();

  if (button & 4) { // Om bit 2 (BTN4) är lika med 1
    mytime &= 0xffff0fff; // Nollställ siffran i fråga
    mytime |= switches << 12; // Skriv in till rätt position
  }

  if (button & 2) { // Om bit 1 (BTN3) är lika med 1
    mytime &= 0xfffff0ff;
    mytime |= switches << 8;
  }

  if (button & 1) { // Om bit 0 (BTN2) är lika med 1
    mytime &= 0xffffff0f;
    mytime |= switches << 4;
  }

  delay(1000);
  time2string(textstring, mytime);
  display_string(3, textstring);
  display_update();
  tick(&mytime);
  display_image(96, icon);

  // Uppg. 1d
  (*porte)++; // Inkrementera innehållet av registret PORTE
}
