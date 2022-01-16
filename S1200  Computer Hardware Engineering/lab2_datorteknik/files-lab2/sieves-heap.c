#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define COLUMNS 6
#define PRINT 1
#define DONT_PRINT 0

int counter = 0;

void print_number(int n);
void sieves(int n, _Bool print);

int delay (int seconds) {

    int n = 2;

    clock_t end_time = clock() + seconds*CLOCKS_PER_SEC;

    while (clock()<end_time)
        sieves(n++, DONT_PRINT);
    
    return n;
}

int main () {

    sieves(97, PRINT);

    int n2 = delay(2);
    printf("2 sekunder = %d\n", n2); // OUTPUT: 2 sekunder = 25647

    int n10 = delay(10);
    printf("10 sekunder = %d\n", n10); // OUTPUT: 10 sekunder = 58622

    return 0;
}

void print_number(int n) {

  if (counter % COLUMNS == 0)
    printf("\n");

  printf("%10d", n);
  counter++;
}

void sieves(int n, _Bool print) {

    int len = n + 1;
    int *sequence = (int *) malloc(len * sizeof(int));
    for (int i = 0; i < len; i++)
        sequence[i] = i;

    for (int i = 2; i < sqrt(len); i++)
        if(sequence[i])
            for (int j = i * i; j < len; j += i)
                sequence[j] = 0;
    
    if (print) {
        for (int i = 2; i < len; i++)
            if (sequence[i])
                print_number(sequence[i]);
        
        printf("\n");
    }

    free(sequence);
}