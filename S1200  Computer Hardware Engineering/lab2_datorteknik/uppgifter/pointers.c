#include <stdio.h>
#include <stdlib.h>


char* text1 = "This is a string.";
char* text2 = "Yet another thing.";
int *list1 = NULL;// = (int*) malloc(80 * sizeof(int));
int *list2 = NULL;// = (int*) malloc(80 * sizeof(int));
int count = 0;


void work(void);
void copycodes(const char* text, int* list);


void printlist(const int* list) {

  printf("ASCII codes and corresponding characters.\n");
  int i = 1;
  while (*list != 0) {
    printf("0x%03x '%c'", *list, (char) *list);
    list++;
    if (i % 6 == 0) printf("\n"); i++;
  }
  printf("\n");
}

void endian_proof(const char* c) {
  
  printf("\nEndian experiment: 0x%02x,0x%02x,0x%02x,0x%02x\n", (int) *c, (int) *(c+1), (int) *(c+2), (int) *(c+3));
}

int main(void) {

  list1 = (int*) malloc(80 * sizeof(int));
  list2 = (int*) malloc(80 * sizeof(int));

  work();

  printf("\nlist1: ");
  printlist(list1);
  printf("\nlist2: ");
  printlist(list2);
  printf("\nCount = %d\n", count);

  endian_proof((char*) &count);
}

void work(void) {

  copycodes(text1, list1);
  copycodes(text2, list2);
}

void copycodes(const char* text, int* list) {

  int i = 0;

  for (; *(text + i) != 0; i++) {
    *(list + i) = *(text + i);
    count++;
  }

  *(list + i) = *(text + i);
}