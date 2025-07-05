/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zones.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 00:00:00 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 15:24:59 by anasshouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>

#include "../includes/malloc.h"

static size_t	get_zone_allocation_size(size_t size, t_zone_type type)
{
	size_t	zone_size;
	size_t	page_size;

	page_size = get_page_size();
	if (type == TINY)
		zone_size = g_tiny_zone_size;
	else if (type == SMALL)
		zone_size = g_small_zone_size;
	else
		zone_size = ((size + sizeof(t_zone) + sizeof(t_block) + page_size - 1)
				/ page_size)
			* page_size;
	return (zone_size);
}

t_zone	*create_zone(size_t size, t_zone_type type)
{
	t_zone	*zone;
	t_block	*block;
	size_t	zone_size;

	zone_size = get_zone_allocation_size(size, type);
	zone = (t_zone *)mmap(NULL, zone_size, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (zone == MAP_FAILED)
		return (NULL);
	zone->size = zone_size;
	zone->type = type;
	zone->next = NULL;
	block = (t_block *)((char *)zone + sizeof(t_zone));
	block->size = zone_size - sizeof(t_zone) - sizeof(t_block);
	block->free = true;
	block->next = NULL;
	block->prev = NULL;
	zone->blocks = block;
	return (zone);
}

void	add_zone(t_zone *zone, t_zone_type type)
{
	t_zone	**zone_list;

	if (type == TINY)
		zone_list = &g_memory.tiny;
	else if (type == SMALL)
		zone_list = &g_memory.small;
	else
		zone_list = &g_memory.large;
	zone->next = *zone_list;
	*zone_list = zone;
}
