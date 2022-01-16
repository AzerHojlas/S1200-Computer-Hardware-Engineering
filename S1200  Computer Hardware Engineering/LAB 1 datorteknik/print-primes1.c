/*
 print-prime.c 
 By David Broman.
 Last modified: 2015-09-15
 This file is in the public domain.
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define COLUMNS 6

int counter = 0;

// 'argc' contains the number of program arguments, and
// 'argv' is an array of char pointers, where each
// char pointer points to a null-terminated string.
int main(int argc, char *argv[]){
  if(argc == 2)
    print_primes(atoi(argv[1]));
  else
    printf("Please state an interger number.\n");
  return 0;
}

 
int is_prime(int n) {
  for (int i=2; i <= sqrt(n); i++) {
    if (n % i == 0)
      return 0;
  }
  return 1;
}

void print_number (int number) {
  if (counter % COLUMNS == 0)
    printf ("\n");

  printf("%10d ", number);
  counter++;
}

void print_primes (int candidates) {
  for (int i = 2; i <= candidates; i++) {
    if (is_prime(i))
      print_number(i);
  }
}