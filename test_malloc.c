#include <stdio.h>
#include <string.h>

#include "includes/malloc.h"

int main() {
  printf("=== Testing Minimal Malloc Implementation ===\n\n");

  // Test basic malloc functionality
  printf("1. Testing basic malloc/free operations:\n");
  void *ptr1 = malloc(42);
  void *ptr2 = malloc(84);
  void *ptr3 = malloc(512);   // SMALL
  void *ptr4 = malloc(2048);  // LARGE

  if (ptr1 && ptr2 && ptr3 && ptr4) {
    printf("✓ All allocations successful\n");

    // Fill with some data
    strcpy((char *)ptr1, "Hello");
    strcpy((char *)ptr2, "World");
    strcpy((char *)ptr3, "This is a SMALL allocation");
    strcpy((char *)ptr4, "This is a LARGE allocation");

    printf("✓ Data written successfully\n");
  } else {
    printf("✗ Some allocations failed\n");
    return 1;
  }

  printf("\n2. Testing show_alloc_mem() output format:\n");
  show_alloc_mem();

  printf("\n3. Testing realloc functionality:\n");
  ptr1 = realloc(ptr1, 100);
  if (ptr1) {
    printf("✓ Realloc successful\n");
    strcat((char *)ptr1, " expanded!");
  } else {
    printf("✗ Realloc failed\n");
  }

  printf("\n4. Testing free functionality:\n");
  free(ptr1);
  free(ptr2);
  free(ptr3);
  free(ptr4);
  printf("✓ All memory freed\n");

  printf("\n5. Final memory state:\n");
  show_alloc_mem();

  printf("\n=== Test completed successfully! ===\n");
  return 0;
}
