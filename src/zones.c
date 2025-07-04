/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zones.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouari <ahouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 18:44:17 by ahouari       #+#    #+#             */
/*   Updated: 2025/07/04 18:44:17 by ahouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <sys/mman.h>

#include "../includes/malloc.h"

/**
 * Calculate zone size based on type and requested size
 * According to spec: zones must be multiples of page size
 */
static size_t calculate_zone_size(size_t size, t_zone_type type) {
  if (type == TINY)
    return g_tiny_zone_size;
  else if (type == SMALL)
    return g_small_zone_size;
  else {
    // LARGE allocations: use mmap for individual allocation
    size_t page_size = GET_PAGE_SIZE();
    size_t total_size = size + sizeof(t_zone) + sizeof(t_block);
    return ((total_size + page_size - 1) / page_size) * page_size;
  }
}

/**
 * Initialize zone metadata
 */
static void init_zone_metadata(t_zone *zone, size_t zone_size,
                               t_zone_type type) {
  zone->type = type;
  zone->size = zone_size;
  zone->next = NULL;
}

/**
 * Initialize first block in zone
 */
static void init_first_block(t_zone *zone, size_t zone_size) {
  t_block *block;

  block = (t_block *)((char *)zone + sizeof(t_zone));
  block->size = zone_size - sizeof(t_zone) - sizeof(t_block);
  block->free = true;
  block->next = NULL;
  block->prev = NULL;

  zone->blocks = block;
}

t_zone *create_zone(size_t size, t_zone_type type) {
  size_t zone_size;
  t_zone *zone;
  void *addr;

  zone_size = calculate_zone_size(size, type);

  addr = mmap(NULL, zone_size, PROT_READ | PROT_WRITE,
              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (addr == MAP_FAILED) return NULL;

  zone = (t_zone *)addr;
  init_zone_metadata(zone, zone_size, type);
  init_first_block(zone, zone_size);

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