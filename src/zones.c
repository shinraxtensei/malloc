#include "../includes/malloc.h"
#include <stddef.h>
#include <sys/mman.h>

t_zone *create_zone(size_t size, t_zone_type type) {
  size_t zone_size;
  t_zone *zone;
  t_block *block;
  void *addr;

  if (type == TINY)
    zone_size = ALIGN(TINY_ZONE_SIZE);
  else if (type == SMALL)
    zone_size = ALIGN(SMALL_ZONE_SIZE);
  else
    zone_size = ALIGN(size + sizeof(t_zone) + sizeof(t_block));

  // Allocate memory for the zone using mmap
  addr = mmap(NULL, zone_size, PROT_READ | PROT_WRITE,
              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (addr == MAP_FAILED)
    return NULL;

  // Zone setup metadata
  zone = (t_zone *)addr;
  zone->type = type;
  zone->size = zone_size;
  zone->next = NULL;

  // Setup the first block in the zone
  block = (t_block *)((char *)addr + sizeof(t_zone));
  block->size = zone_size - sizeof(t_zone) - sizeof(t_block);
  block->free = true;
  block->next = NULL;
  block->prev = NULL;

  zone->blocks = block;
  return zone;
}

/**
 * Add a zone to the appropriate list in the memory manager
 */
void add_zone(t_zone *zone, t_zone_type type) {
  t_zone **zone_list;
  
  if (type == TINY)
    zone_list = &g_memory.tiny;
  else if (type == SMALL)
    zone_list = &g_memory.small;
  else
    zone_list = &g_memory.large;

  zone->next = *zone_list;
  *zone_list = zone;
}

/**
 * Get the appropriate zone type for the given size
 */
t_zone_type get_zone_type(size_t size) {
  if (size <= TINY_MAX)
    return TINY;
  else if (size <= SMALL_MAX)
    return SMALL;
  else
    return LARGE;
}