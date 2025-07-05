/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_statistics.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouari <ahouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 12:00:00 by ahouari          #+#    #+#             */
/*   Updated: 2025/07/05 18:59:11 by ahouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "../includes/malloc.h"

// Display detailed zone information with hex dumps
static void	show_zone_ex(t_zone *zone, const char *type_name)
{
	if (!zone)
		return ;
	printf("%s zones:\n", type_name);
	while (zone)
	{
		// Print detailed zone information
		print_zone_blocks(zone);
		zone = zone->next;
	}
}

// Count total allocated memory in a zone list
static size_t	count_zone_memory(t_zone *zone)
{
	t_block	*block;
	size_t	total;

	total = 0;
	while (zone)
	{
		// Check all blocks in this zone
		block = zone->blocks;
		while (block)
		{
			// Only count allocated blocks
			if (!block->free)
				total += block->size;
			block = block->next;
		}
		zone = zone->next;
	}
	return (total);
}

// Calculate total allocated memory across all zones
size_t	calculate_total_memory(void)
{
	size_t	total;

	total = 0;
	// Sum memory from all zone types
	total += count_zone_memory(g_memory.tiny);
	total += count_zone_memory(g_memory.small);
	total += count_zone_memory(g_memory.large);
	return (total);
}

// Extended memory display with detailed zone information and total statistics
void	show_alloc_mem_ex(void)
{
	size_t	total;

	// Thread-safe display of all memory zones
	pthread_mutex_lock(&g_malloc_mutex);
	show_zone_ex(g_memory.tiny, "TINY");
	show_zone_ex(g_memory.small, "SMALL");
	show_zone_ex(g_memory.large, "LARGE");
	total = calculate_total_memory();
	printf("Total allocated memory: %zu bytes\n", total);
	pthread_mutex_unlock(&g_malloc_mutex);
}
