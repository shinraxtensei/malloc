
#include "../includes/malloc.h"

bool is_valid_pointer(void *ptr, t_block **block_prt, t_zone **zone_ptr) {
  t_zone *zone;
  t_block *block;
  t_zone_type type;

  for (type = TINY; type <= LARGE; type++) {
    // if (type == TINY)
    //   zone = g_memory.tiny;
    // else if (type == SMALL)
    //   zone = g_memory.small;
    // else
    //   zone = g_memory.large;
    zone = get_zone_list(type);
    while (zone) {
      if (ptr > (void *)zone && ptr < (void *)((char *)zone + zone->size)) {
        block = zone->blocks;
        while (block) {

          void *block_data = (void *)((char *)block + sizeof(t_block));
          if (ptr == block_data) {
            *block_prt = block;
            *zone_ptr = zone;
            return true;
          }
          block = block->next;
        }
      }
      zone = zone->next;
    }
  }
  return false;
}
