#include <stdio.h>

#include "../includes/malloc.h"

/**
 * Display blocks in a specific zone with exact specification format
 */
static void show_zone_blocks(t_zone *zone, const char *zone_name) {
  t_block *block;
  bool first_zone = true;

  while (zone) {
    if (first_zone) {
      printf("%s : %p\n", zone_name, (void *)zone);
      first_zone = false;
    }

    block = zone->blocks;
    while (block) {
      if (!block->free) {
        void *start = (void *)((char *)block + sizeof(t_block));
        void *end = (void *)((char *)start + block->size - 1);
        printf("%p - %p : %zu bytes\n", start, end, block->size);
      }
      block = block->next;
    }
    zone = zone->next;
  }
}

/**
 * Calculate total allocated memory across all zones
 */
static size_t calculate_total_memory(void) {
  t_zone *zone;
  t_block *block;
  size_t total = 0;
  t_zone_type type;

  for (type = TINY; type <= LARGE; type++) {
    zone = get_zone_list(type);
    while (zone) {
      block = zone->blocks;
      while (block) {
        if (!block->free) {
          total += block->size;
        }
        block = block->next;
      }
      zone = zone->next;
    }
  }
  return total;
}

/**
 * Display memory allocation information as required by specification
 */
void show_alloc_mem(void) {
  size_t total;

  pthread_mutex_lock(&g_memory.mutex);

  show_zone_blocks(g_memory.tiny, "TINY");
  show_zone_blocks(g_memory.small, "SMALL");
  show_zone_blocks(g_memory.large, "LARGE");

  total = calculate_total_memory();
  printf("Total : %zu bytes\n", total);

  pthread_mutex_unlock(&g_memory.mutex);
}
