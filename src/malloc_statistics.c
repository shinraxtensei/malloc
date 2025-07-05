/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_statistics.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 12:00:00 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 15:25:02 by anasshouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "../includes/malloc.h"

static void	show_zone_ex(t_zone *zone, const char *type_name)
{
	if (!zone)
		return ;
	printf("%s zones:\n", type_name);
	while (zone)
	{
		print_zone_blocks(zone);
		zone = zone->next;
	}
}

static size_t	count_zone_memory(t_zone *zone)
{
	t_block	*block;
	size_t	total;

	total = 0;
	while (zone)
	{
		block = zone->blocks;
		while (block)
		{
			if (!block->free)
				total += block->size;
			block = block->next;
		}
		zone = zone->next;
	}
	return (total);
}

size_t	calculate_total_memory(void)
{
	size_t	total;

	total = 0;
	total += count_zone_memory(g_memory.tiny);
	total += count_zone_memory(g_memory.small);
	total += count_zone_memory(g_memory.large);
	return (total);
}

void	show_alloc_mem_ex(void)
{
	size_t	total;

	pthread_mutex_lock(&g_memory.mutex);
	show_zone_ex(g_memory.tiny, "TINY");
	show_zone_ex(g_memory.small, "SMALL");
	show_zone_ex(g_memory.large, "LARGE");
	total = calculate_total_memory();
	printf("Total allocated memory: %zu bytes\n", total);
	pthread_mutex_unlock(&g_memory.mutex);
}
