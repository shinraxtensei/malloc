/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 00:00:00 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 15:25:01 by anasshouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <sys/mman.h>

#include "../includes/malloc.h"

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
	memcpy(new_ptr, ptr, copy_size);
	free(ptr);
	return (new_ptr);
}

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

void	*realloc(void *ptr, size_t size)
{
	t_block	*block;
	t_zone	*zone;
	size_t	aligned_size;
	void	*result;

	result = handle_realloc_edge_cases(ptr, size);
	if (result != ptr)
		return (result);
	init_memory_manager();
	pthread_mutex_lock(&g_memory.mutex);
	aligned_size = align_size(size);
	if (!is_valid_pointer(ptr, &block, &zone))
	{
		pthread_mutex_unlock(&g_memory.mutex);
		return (NULL);
	}
	if (try_expand_in_place(block, zone, aligned_size))
	{
		pthread_mutex_unlock(&g_memory.mutex);
		return (ptr);
	}
	pthread_mutex_unlock(&g_memory.mutex);
	return (realloc_with_copy(ptr, size, block));
}
