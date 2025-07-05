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
 * Display memory allocation information
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

/**
 * Generate zone node identifier for Mermaid
 */
static void print_zone_id(t_zone *zone, t_zone_type type) {
  printf("Z%s_%p",
         (type == TINY)    ? "TINY"
         : (type == SMALL) ? "SMALL"
                           : "LARGE",
         (void *)zone);
}

/**
 * Generate block node identifier for Mermaid
 */
static void print_block_id(t_block *block) { printf("B_%p", (void *)block); }

/**
 * Extended memory allocation information with hex dump
 * Bonus function mentioned in the specification
 */
void show_alloc_mem_ex(void) {
  t_zone *zone;
  t_block *block;
  t_zone_type type;
  size_t total = 0;
  int allocation_count = 0;

  pthread_mutex_lock(&g_memory.mutex);

  printf("=== Extended Memory Allocation Information ===\n");

  for (type = TINY; type <= LARGE; type++) {
    const char *type_name = (type == TINY)    ? "TINY"
                            : (type == SMALL) ? "SMALL"
                                              : "LARGE";
    zone = get_zone_list(type);

    if (zone) {
      printf("\n%s zones:\n", type_name);

      while (zone) {
        printf("  Zone %p (size: %zu bytes)\n", (void *)zone, zone->size);
        block = zone->blocks;

        while (block) {
          if (!block->free) {
            void *start = (void *)((char *)block + sizeof(t_block));
            void *end = (void *)((char *)start + block->size - 1);
            printf("    %p - %p : %zu bytes", start, end, block->size);

            // Show hex dump of first 32 bytes or less
            size_t dump_size = (block->size < 32) ? block->size : 32;
            printf(" [");
            for (size_t i = 0; i < dump_size; i++) {
              printf("%02x", ((unsigned char *)start)[i]);
              if (i < dump_size - 1) printf(" ");
            }
            if (block->size > 32) printf("...");
            printf("]\n");

            total += block->size;
            allocation_count++;
          }
          block = block->next;
        }
        zone = zone->next;
      }
    } else {
      printf("\n%s zones: (empty)\n", type_name);
    }
  }

  printf("\nSummary:\n");
  printf("Total allocations: %d\n", allocation_count);
  printf("Total allocated memory: %zu bytes\n", total);

  pthread_mutex_unlock(&g_memory.mutex);
}