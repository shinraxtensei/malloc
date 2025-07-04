/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_show_alloc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouari <ahouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 18:46:20 by ahouari       #+#    #+#             */
/*   Updated: 2025/07/04 18:46:20 by ahouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

#include "includes/malloc.h"

int main() {
  printf("=== Testing show_alloc_mem format ===\n");

  // Make some allocations to test the output format
  void *ptr1 = malloc(42);    // TINY
  void *ptr2 = malloc(84);    // TINY
  void *ptr3 = malloc(512);   // SMALL
  void *ptr4 = malloc(3000);  // LARGE

  // Fill with some data
  strcpy((char *)ptr1, "Hello");
  strcpy((char *)ptr2, "World");
  strcpy((char *)ptr3, "This is a SMALL allocation");
  strcpy((char *)ptr4, "This is a LARGE allocation with more data");

  printf("\nCalling show_alloc_mem():\n");
  show_alloc_mem();

  printf("\nCalling show_alloc_mem_ex():\n");
  show_alloc_mem_ex();

  // Cleanup
  free(ptr1);
  free(ptr2);
  free(ptr3);
  free(ptr4);

  printf("\nAfter freeing all allocations:\n");
  show_alloc_mem();

  return 0;
}
