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

	if (block->next && block->next->free)
	{
		total_size = block->size + sizeof(t_block) + block->next->size;
		if (total_size >= size)
		{
			coalesce_blocks(zone, block);
			if (block->size >= size)
			{
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

	new_ptr = malloc(size);
	if (new_ptr == NULL)
		return (NULL);
	if (block->size < size)
		copy_size = block->size;
	else
		copy_size = size;
	ft_memcpy(new_ptr, ptr, copy_size);
	free(ptr);
	return (new_ptr);
}

// Handle special cases for realloc
static void	*handle_realloc_edge_cases(void *ptr, size_t size)
{
	if (ptr == NULL)
		return (malloc(size));
	if (size == 0)
	{
		free(ptr);
		return (NULL);
	}
	return (ptr);
}

// Resize allocated memory block
static void	*realloc_validated_pointer(void *ptr, size_t size,
		t_block *block, t_zone *zone)
{
	size_t	aligned_size;

	aligned_size = align_size(size);
	if (try_expand_in_place(block, zone, aligned_size))
	{
		pthread_mutex_unlock(&g_malloc_mutex);
		return (ptr);
	}
	pthread_mutex_unlock(&g_malloc_mutex);
	return (realloc_with_copy(ptr, size, block));
}

void	*realloc(void *ptr, size_t size)
{
	t_block	*block;
	t_zone	*zone;
	void	*result;

	result = handle_realloc_edge_cases(ptr, size);
	if (result != ptr)
		return (result);
	init_memory_manager();
	pthread_mutex_lock(&g_malloc_mutex);
	if (!is_valid_pointer(ptr, &block, &zone))
	{
		pthread_mutex_unlock(&g_malloc_mutex);
		return (NULL);
	}
	return (realloc_validated_pointer(ptr, size, block, zone));
}
