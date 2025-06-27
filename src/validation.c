
#include "../includes/malloc.h"

/**
 * Check if pointer is within zone bounds
 */
static bool is_pointer_in_zone(void *ptr, t_zone *zone) {
  return (ptr > (void *)zone && ptr < (void *)((char *)zone + zone->size));
}

/**
 * Find block matching the given pointer in a zone
 */
static bool find_block_in_zone(void *ptr, t_zone *zone, t_block **block_ptr) {
  t_block *block = zone->blocks;

  while (block) {
    void *block_data = (void *)((char *)block + sizeof(t_block));
    if (ptr == block_data) {
      *block_ptr = block;
      return true;
    }
    block = block->next;
  }
  return false;
}

bool is_valid_pointer(void *ptr, t_block **block_ptr, t_zone **zone_ptr) {
  t_zone *zone;
  t_zone_type type;

  for (type = TINY; type <= LARGE; type++) {
    zone = get_zone_list(type);
    while (zone) {
      if (is_pointer_in_zone(ptr, zone)) {
        if (find_block_in_zone(ptr, zone, block_ptr)) {
          *zone_ptr = zone;
          return true;
        }
      }
      zone = zone->next;
    }
  }
  return false;
}
