#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

// Uppg. 1f
int getsw(void) {

    return (int) ((PORTD & 0xf00) >> 8); 
}

// Uppg. 1g
int getbtns(void) {

    return (int) ((PORTD & 0xe0) >> 5);
}