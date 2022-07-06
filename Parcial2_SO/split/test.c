#include <stdio.h>
#include <stdlib.h>
#include "split.h"

int main(int argc, char * argv[]) {
  int n;
  char ** result;
  int i;
  char * delim;
  
  if (argc == 1) {
    fprintf(stderr, "Debe especificar una cadena\n");
    exit(EXIT_FAILURE);
  }

  if (argc >= 3) {
    delim = argv[2];
  }else {
    delim = " \t\r\n";
  }


  n = 0;
  result = split(argv[1], delim, &n);
  printf("Tokens: %d\n", n);
  for (i = 0; i < n; i++) {
    printf("%d: %s\n", i, result[i]);
    free(result[i]);
    result[i] = 0;
  }
  free(result);
}
