/*
 print-prime.c
 By David Broman.
 Last modified: 2015-09-15
 This file is in the public domain.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h> // Okej?
#include <time.h>
#define COLUMNS 6
#define PRINT 1
#define DONT_PRINT 0

int counter = 0;

int is_prime(int n);
void print_number(int n, _Bool print);
void print_primes(int n, _Bool print);

int delay (int seconds) {

    int n = 2;

    clock_t end_time = clock() + seconds*CLOCKS_PER_SEC;

    while (clock()<end_time)
        print_primes(n++, DONT_PRINT);
    
    return n;
}

// 'argc' contains the number of program arguments, and
// 'argv' is an array of char pointers, where each
// char pointer points to a null-terminated string.
int main(int argc, char *argv[]){
  if(argc == 2)
    print_primes(atoi(argv[1]), PRINT);
  else
    printf("Please state an interger number.\n");

  int n2 = delay(2);
  printf("2 sekunder = %d\n", n2); // OUTPUT: 2 sekunde = 3359

  int n10 = delay(10);
  printf("10 sekunder = %d\n", n10); // OUTPUT: 10 sekunder = 7060
  
  return 0;
}

int is_prime(int n) {

  for (int i = 2; i <= sqrt(n); i++)
    if (n % i == 0)
      return 0;

  return 1;
}

void print_number(int n, _Bool print) { // Bör reglering av kolumner ske i denna funktion?

  if (!print) return;

  if (counter % COLUMNS == 0)
    printf("\n");

  printf("%10d", n);
  counter++;
}

void print_primes(int n, _Bool print) {

  for (int i = 2; i <= n; i++)
    if (is_prime(i))
      print_number(i, print);
  
  printf("\n");
}
