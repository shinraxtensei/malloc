#include <stdio.h>
#include <stdlib.h>

void show_alloc_mem(void);

int main(void) {
  char *ptr;
  char *new_ptr;

  // Test realloc with NULL pointer (should behave like malloc)
  ptr = realloc(NULL, 1024);
  if (ptr) {
    printf("Bonjour\n");

    // Test realloc with size 0 (should behave like free)
    new_ptr = realloc(ptr, 0);
    if (new_ptr == NULL) {
      printf("Realloc with size 0 returned NULL correctly\n");
    }
  }

  return (0);
}
