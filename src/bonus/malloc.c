#include "../includes/malloc.h"

#include <_string.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

/* Global zone size variables - calculated at runtime */
size_t g_tiny_zone_size = 0;
size_t g_small_zone_size = 0;

/* Global memory manager instance -  */
t_memory g_memory = {.tiny = NULL,
                     .small = NULL,
                     .large = NULL,
                     .mutex = PTHREAD_MUTEX_INITIALIZER};

/* Global boolean to track initialization */
static bool g_initialized = false;

/**
 * Calculate zone sizes based on page size (spec requirement)
 */
static void calculate_zone_sizes(void) {
  size_t page_size;
  size_t tiny_needed;
  size_t small_needed;

  if (g_tiny_zone_size != 0) return;  // Already calculated

  page_size = GET_PAGE_SIZE();

  // Calculate minimum size needed for 100 allocations + metadata
  tiny_needed = (TINY_MAX + sizeof(t_block)) * TINY_ZONE_ALLOCS;
  small_needed = (SMALL_MAX + sizeof(t_block)) * SMALL_ZONE_ALLOCS;

  // Round up to multiple of page size
  g_tiny_zone_size = ((tiny_needed + page_size - 1) / page_size) * page_size;
  g_small_zone_size = ((small_needed + page_size - 1) / page_size) * page_size;
}

/**
 * Initialize the memory manager (called once)
 */
static void init_memory_manager(void) {
  if (!g_initialized) {
    calculate_zone_sizes();
    pthread_mutex_init(&g_memory.mutex, NULL);
    g_initialized = true;
  }
}

/**
 * Allocate memory from a specific zone
 */
static void *allocate_from_zone(t_zone *zone, size_t size) {
  t_block *block;

  block = find_free_block(zone, size);
  if (!block) return NULL;

  block = split_block(block, size);
  block->free = false;

  return (void *)((char *)block + sizeof(t_block));
}

/**
 * Allocate a memory block of the specified size
 */
static void *allocate_memory(size_t size) {
  t_zone *zone;
  t_zone *new_zone;
  void *ptr;
  t_zone_type type;

  type = get_zone_type(size);

  // Get the appropriate zone list
  if (type == TINY)
    zone = g_memory.tiny;
  else if (type == SMALL)
    zone = g_memory.small;
  else
    zone = g_memory.large;

  zone = get_zone_list(type);

  // Try to find free space in existing zones
  while (zone) {
    ptr = allocate_from_zone(zone, size);
    if (ptr) return ptr;
    zone = zone->next;
  }

  // No space found, create new zone
  new_zone = create_zone(size, type);
  if (!new_zone) return NULL;

  add_zone(new_zone, type);

  return allocate_from_zone(new_zone, size);
}

void *malloc(size_t size) {
  void *ptr;
  size_t aligned_size;

  if (size == 0) return NULL;

  init_memory_manager();

  aligned_size = ALIGN(size);

  pthread_mutex_lock(&g_memory.mutex);
  ptr = allocate_memory(aligned_size);
  pthread_mutex_unlock(&g_memory.mutex);
  return ptr;
}

// free

/**
 * Handle cleanup of large zones when completely freed
 */
static void cleanup_large_zone(t_zone *zone) {
  if (zone->type == LARGE && zone->blocks->free && zone->blocks->next == NULL) {
    if (g_memory.large == zone)
      g_memory.large = zone->next;
    else {
      t_zone *prev = g_memory.large;
      while (prev && prev->next != zone) prev = prev->next;
      if (prev) prev->next = zone->next;
    }
    munmap(zone, zone->size);
  }
}

void free(void *ptr) {
  t_block *block;
  t_zone *zone;

  if (ptr == NULL) return;

  init_memory_manager();
  pthread_mutex_lock(&g_memory.mutex);

  if (is_valid_pointer(ptr, &block, &zone)) {
    block->free = true;
    coalesce_blocks(zone, block);
    cleanup_large_zone(zone);
  }

  pthread_mutex_unlock(&g_memory.mutex);
}

/**
 * Try to expand block in place by coalescing with next free block
 */
static bool try_expand_in_place(t_block *block, t_zone *zone,
                                size_t aligned_size) {
  if (block->next && block->next->free &&
      block->size + sizeof(t_block) + block->next->size >= aligned_size) {
    coalesce_blocks(zone, block);
    if (block->size >= aligned_size) {
      split_block(block, aligned_size);
      return true;
    }
  }
  return false;
}

/**
 * Handle realloc by copying to new allocation
 */
static void *realloc_with_copy(void *ptr, size_t size, t_block *block) {
  void *new_ptr;
  size_t copy_size;

  new_ptr = malloc(size);
  if (new_ptr == NULL) return NULL;

  copy_size = (block->size < size) ? block->size : size;
  memcpy(new_ptr, ptr, copy_size);
  free(ptr);

  return new_ptr;
}

void *realloc(void *ptr, size_t size) {
  t_block *block;
  t_zone *zone;
  size_t aligned_size;

  if (ptr == NULL) return malloc(size);
  if (size == 0) {
    free(ptr);
    return NULL;
  }

  init_memory_manager();
  aligned_size = ALIGN(size);

  pthread_mutex_lock(&g_memory.mutex);

  if (!is_valid_pointer(ptr, &block, &zone)) {
    pthread_mutex_unlock(&g_memory.mutex);
    return NULL;
  }

  if (try_expand_in_place(block, zone, aligned_size)) {
    pthread_mutex_unlock(&g_memory.mutex);
    return ptr;
  }

  pthread_mutex_unlock(&g_memory.mutex);
  return realloc_with_copy(ptr, size, block);
}
