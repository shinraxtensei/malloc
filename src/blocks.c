#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>

#include "../includes/malloc.h"

t_block *find_free_block(t_zone *zone, size_t size) {
  t_block *block = zone->blocks;

  while (block) {
    if (block->free && block->size >= size) return block;
    block = block->next;
  }
  return NULL;
}

t_block *split_block(t_block *block, size_t size) {
  t_block *new_block;
  size_t remaining_size;

  if (block->size < size + sizeof(t_block) + ALIGN_SIZE) return block;

  remaining_size = block->size - size - sizeof(t_block);

  // Create a new block from the excess space
  new_block = (t_block *)((char *)block + sizeof(t_block) + size);
  new_block->size = remaining_size;
  new_block->free = true;
  new_block->next = block->next;
  new_block->prev = block;

  // Update linked list
  if (block->next) block->next->prev = new_block;
  block->next = new_block;
  block->size = size;

  return block;
}

void coalesce_blocks(t_zone *zone, t_block *block) {
  t_block *next_block;
  (void)zone;  // Suppress unused parameter warning

  if (!block->free) return;

  next_block = block->next;
  // Try to merge with the next block if it's free
  while (next_block && next_block->free) {
    block->size +=
        next_block->size + sizeof(t_block);  // Fixed: += instead of =
    block->next = next_block->next;
    if (next_block->next) next_block->next->prev = block;

    next_block = next_block->next;
  }
}