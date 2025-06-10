#include "../includes/malloc.h"
#include <_string.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

/* Global memory manager instance -  */
t_memory g_memory = {.tiny = NULL,
                     .small = NULL,
                     .large = NULL,
                     .mutex = PTHREAD_MUTEX_INITIALIZER};

/* Global boolean to track initialization */
static bool g_initialized = false;

/**
 * Initialize the memory manager (called once)
 */
static void init_memory_manager(void) {
  if (!g_initialized) {
    pthread_mutex_init(&g_memory.mutex, NULL);
    g_initialized = true;
  }
}

static t_zone *get_zone_list(t_zone_type type) {
  if (type == TINY)
    return g_memory.tiny;
  else if (type == SMALL)
    return g_memory.small;
  else
    return g_memory.large;
}

/**
 * Allocate memory from a specific zone
 */
static void *allocate_from_zone(t_zone *zone, size_t size) {
  t_block *block;

  block = find_free_block(zone, size);
  if (!block)
    return NULL;

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
    if (ptr)
      return ptr;
    zone = zone->next;
  }

  // No space found, create new zone
  new_zone = create_zone(size, type);
  if (!new_zone)
    return NULL;

  add_zone(new_zone, type);

  return allocate_from_zone(new_zone, size);
}

void *malloc(size_t size) {
  void *ptr;
  size_t aligned_size;

  if (size == 0)
    return NULL;

  init_memory_manager();

  aligned_size = ALIGN(size);

  pthread_mutex_lock(&g_memory.mutex);
  ptr = allocate_memory(aligned_size);
  pthread_mutex_unlock(&g_memory.mutex);
  return ptr;
}

// free

void free(void *ptr) {
  t_block *block;
  t_zone *zone;

  if (ptr == NULL)
    return;

  init_memory_manager();
  pthread_mutex_lock(&g_memory.mutex);

  if (is_valid_pointer(ptr, &block, &zone)) {
    block->free = true;
    coalesce_blocks(zone, block);
    if (zone->type == LARGE && zone->blocks->free &&
        zone->blocks->next == NULL) {
      if (g_memory.large == zone)
        g_memory.large = zone->next;
      else {
        t_zone *prev = g_memory.large;
        while (prev && prev->next != zone)
          prev = prev->next;
        if (prev)
          prev->next = zone->next;
      }
      munmap(zone, zone->size);
    }
  }

  pthread_mutex_unlock(&g_memory.mutex);
}

void *realloc(void *ptr, size_t size) {
  t_block *block;
  t_zone *zone;
  void *new_ptr;
  size_t aligned_size;
  size_t copy_size;

  if (ptr == NULL)
    return malloc(size);
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

  if (block->next && block->next->free &&
      block->size + sizeof(t_block) + block->next->size >= aligned_size) {
    coalesce_blocks(zone, block);

    if (block->size >= aligned_size) {
      split_block(block, aligned_size);
      pthread_mutex_unlock(&g_memory.mutex);
      return ptr;
    }
  }

  pthread_mutex_unlock(&g_memory.mutex);

  new_ptr = malloc(size);
  if (new_ptr == NULL)
    return NULL;

  copy_size = (block->size < size) ? block->size : size;
  memcpy(new_ptr, ptr, copy_size);

  free(ptr);

  return new_ptr;
}

void *calloc(size_t nmemb, size_t size) {
  void *ptr;
  size_t total_size;
  // check for multiplication overflow
  if (nmemb > 0 && size > SIZE_MAX / nmemb)
    return NULL;
  total_size = nmemb * size;

  ptr = malloc(total_size);
  if (ptr == NULL)
    return NULL;

  memset(ptr, 0, total_size);

  return ptr;
}
