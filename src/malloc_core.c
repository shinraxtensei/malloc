/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_core.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 00:00:00 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 18:48:27 by anasshouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>

#include "../includes/malloc.h"

// Allocate memory from a specific zone
static void	*allocate_from_zone(t_zone *zone, size_t size)
{
	t_block	*block;

	// Find a suitable free block
	block = find_free_block(zone, size);
	if (!block)
		return (NULL);
	
	// Split block if it's larger than needed
	block = split_block(block, size);
	
	// Mark block as allocated
	block->free = false;
	
	// Return pointer to usable data (skip metadata)
	return ((void *)((char *)block + sizeof(t_block)));
}

// Try to allocate from existing zones of the given type
static void	*try_existing_zones(size_t aligned_size, t_zone_type type)
{
	t_zone	*zone;
	void	*ptr;

	// Get first zone of requested type
	zone = get_zone_list(type);
	
	// Try each zone until allocation succeeds
	while (zone)
	{
		ptr = allocate_from_zone(zone, aligned_size);
		if (ptr)
			return (ptr);
		zone = zone->next;
	}
	return (NULL);
}

// Create a new zone and allocate from it
static void	*create_and_allocate(size_t aligned_size, t_zone_type type)
{
	t_zone	*zone;
	void	*ptr;

	// Create new zone via mmap
	zone = create_zone(aligned_size, type);
	if (!zone)
		return (NULL);
	
	// Add zone to appropriate list
	add_zone(zone, type);
	
	// Allocate from the new zone
	ptr = allocate_from_zone(zone, aligned_size);
	return (ptr);
}

// Main allocation strategy: try existing zones, then create new ones
void	*allocate_memory(size_t aligned_size)
{
	t_zone_type	type;
	void		*ptr;

	// Determine which zone type to use (TINY/SMALL/LARGE)
	type = get_zone_type(aligned_size);
	
	// First, try to allocate from existing zones
	ptr = try_existing_zones(aligned_size, type);
	if (ptr)
		return (ptr);
	
	// If no space, create a new zone
	ptr = create_and_allocate(aligned_size, type);
	return (ptr);
}

// Free memory and perform cleanup
void	free_memory(void *ptr)
{
	t_block	*block;
	t_zone	*zone;

	// Validate pointer and get block/zone references
	if (is_valid_pointer(ptr, &block, &zone))
	{
		// Mark block as free
		block->free = true;
		
		// Merge adjacent free blocks
		coalesce_blocks(zone, block);
		
		// Clean up large zones if completely free
		cleanup_large_zone(zone);
	}
}
