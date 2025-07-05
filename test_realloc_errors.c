#include <stdio.h>
#include <stdlib.h>

int main(void) {
  char *ptr;
  char *new_ptr;

  ptr = realloc(NULL, 1024);
  if (ptr) {
    printf("Bonjour\n");
    new_ptr = realloc(ptr, 0);
    if (new_ptr == NULL) {
      printf("Success\n");
    }
  }

  return (0);
}
