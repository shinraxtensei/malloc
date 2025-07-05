/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blocks.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouari <ahouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 18:46:00 by ahouari       #+#    #+#             */
/*   Updated: 2025/07/04 18:46:00 by ahouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>

#include "../includes/malloc.h"

// Search for a free block in a zone that can fit the requested size
t_block *find_free_block(t_zone *zone, size_t size) {
  t_block *block = zone->blocks;

  // Traverse the linked list of blocks
  while (block) {
    // Check if block is free and large enough
    if (block->free && block->size >= size) return block;
    block = block->next;
  }
  return NULL;
}

// Split a block into two parts: requested size and remainder
t_block *split_block(t_block *block, size_t size) {
  t_block *new_block;
  size_t remaining_size;

  // Don't split if remainder would be too small to be useful
  if (block->size < size + sizeof(t_block) + ALIGN_SIZE) return block;

  // Calculate size of the new block that will hold the remainder
  remaining_size = block->size - size - sizeof(t_block);

  // Create a new block from the excess space
  new_block = (t_block *)((char *)block + sizeof(t_block) + size);
  new_block->size = remaining_size;
  new_block->free = true;
  new_block->next = block->next;
  new_block->prev = block;

  // Update linked list pointers
  if (block->next) block->next->prev = new_block;
  block->next = new_block;
  block->size = size;

  return block;
}

// Merge adjacent free blocks to reduce fragmentation
void coalesce_blocks(t_zone *zone, t_block *block) {
  t_block *next_block;
  (void)zone;  // Suppress unused parameter warning

  // Only coalesce if current block is free
  if (!block->free) return;

  next_block = block->next;
  // Try to merge with the next block if it's free
  while (next_block && next_block->free) {
    // Add next block's size plus its metadata to current block
    block->size +=
        next_block->size + sizeof(t_block);  // Fixed: += instead of =
    // Skip over the merged block in the linked list
    block->next = next_block->next;
    if (next_block->next) next_block->next->prev = block;

    next_block = next_block->next;
  }
}