/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouari <ahouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 00:00:00 by ahouari          #+#    #+#             */
/*   Updated: 2025/07/05 18:59:10 by ahouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <sys/mman.h>

#include "../includes/malloc.h"

// Try to expand a block in place by merging with adjacent free blocks
static bool	try_expand_in_place(t_block *block, t_zone *zone, size_t size)
{
	size_t	total_size;

	// Check if next block is free and can be merged
	if (block->next && block->next->free)
	{
		// Calculate total available space
		total_size = block->size + sizeof(t_block) + block->next->size;
		if (total_size >= size)
		{
			// Merge blocks
			coalesce_blocks(zone, block);
			
			// Check if merged block is large enough
			if (block->size >= size)
			{
				// Split if too large
				split_block(block, size);
				return (true);
			}
		}
	}
	return (false);
}

// Allocate new memory and copy data when in-place expansion fails
static void	*realloc_with_copy(void *ptr, size_t size, t_block *block)
{
	void	*new_ptr;
	size_t	copy_size;

	// Allocate new memory
	new_ptr = malloc(size);
	if (new_ptr == NULL)
		return (NULL);
	
	// Copy minimum of old size and new size
	if (block->size < size)
		copy_size = block->size;
	else
		copy_size = size;
	
	// Copy data and free old memory
	memcpy(new_ptr, ptr, copy_size);
	free(ptr);
	return (new_ptr);
}

// Handle special cases for realloc
static void	*handle_realloc_edge_cases(void *ptr, size_t size)
{
	// realloc(NULL, size) == malloc(size)
	if (ptr == NULL)
		return (malloc(size));
	
	// realloc(ptr, 0) == free(ptr); return NULL
	if (size == 0)
	{
		free(ptr);
		return (NULL);
	}
	// Normal case: return original pointer unchanged
	return (ptr);
}

// Resize allocated memory block
void	*realloc(void *ptr, size_t size)
{
	t_block	*block;
	t_zone	*zone;
	size_t	aligned_size;
	void	*result;

	// Handle edge cases first
	result = handle_realloc_edge_cases(ptr, size);
	if (result != ptr)
		return (result);
	
	// Initialize system and prepare for reallocation
	init_memory_manager();
	pthread_mutex_lock(&g_malloc_mutex);
	aligned_size = align_size(size);
	
	// Validate the pointer
	if (!is_valid_pointer(ptr, &block, &zone))
	{
		pthread_mutex_unlock(&g_malloc_mutex);
		return (NULL);
	}
	
	// Try to expand in place first (most efficient)
	if (try_expand_in_place(block, zone, aligned_size))
	{
		pthread_mutex_unlock(&g_malloc_mutex);
		return (ptr);
	}
	
	// Can't expand in place, unlock and use copy strategy
	pthread_mutex_unlock(&g_malloc_mutex);
	return (realloc_with_copy(ptr, size, block));
}
