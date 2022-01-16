#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define COLUMNS 6

int counter = 0;

int is_prime(int n);
void print_number (int number);
void print_sieves (int n);



int main(){

  print_sieves(30);

  printf ("\n");

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

void print_sieves (int n) {
    int len = n+1;

    int a [len];

    for (int i = 0; i < len; i++)
        a [i] = i;

    for (int i = 2; i <= sqrt(n); i++){
        if (a [i]){
            for (int j = i*i; j <= n; j += i)
               a [j] = 0;
        }

    }
    for (int i = 2; i < len; i++) {
        if (a [i])
            print_number (a[i]);
    }
}   
