/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_core.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 00:00:00 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 15:25:02 by anasshouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>

#include "../includes/malloc.h"

static void	*allocate_from_zone(t_zone *zone, size_t size)
{
	t_block	*block;

	block = find_free_block(zone, size);
	if (!block)
		return (NULL);
	block = split_block(block, size);
	block->free = false;
	return ((void *)((char *)block + sizeof(t_block)));
}

static void	*try_existing_zones(size_t aligned_size, t_zone_type type)
{
	t_zone	*zone;
	void	*ptr;

	zone = get_zone_list(type);
	while (zone)
	{
		ptr = allocate_from_zone(zone, aligned_size);
		if (ptr)
			return (ptr);
		zone = zone->next;
	}
	return (NULL);
}

static void	*create_and_allocate(size_t aligned_size, t_zone_type type)
{
	t_zone	*zone;
	void	*ptr;

	zone = create_zone(aligned_size, type);
	if (!zone)
		return (NULL);
	add_zone(zone, type);
	ptr = allocate_from_zone(zone, aligned_size);
	return (ptr);
}

void	*allocate_memory(size_t aligned_size)
{
	t_zone_type	type;
	void		*ptr;

	type = get_zone_type(aligned_size);
	ptr = try_existing_zones(aligned_size, type);
	if (ptr)
		return (ptr);
	ptr = create_and_allocate(aligned_size, type);
	return (ptr);
}

void	free_memory(void *ptr)
{
	t_block	*block;
	t_zone	*zone;

	if (is_valid_pointer(ptr, &block, &zone))
	{
		block->free = true;
		coalesce_blocks(zone, block);
		cleanup_large_zone(zone);
	}
}
