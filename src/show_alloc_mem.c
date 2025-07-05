/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 18:45:24 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 18:48:32 by anasshouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "../includes/malloc.h"

// Print information about an allocated block
static void	print_block_info(t_block *block)
{
	void	*start;
	void	*end;

	// Only print allocated blocks
	if (!block->free)
	{
		// Calculate user data range
		start = (void *)((char *)block + sizeof(t_block));
		end = (void *)((char *)start + block->size - 1);
		printf("%p - %p : %zu bytes\n", start, end, block->size);
	}
}

// Display all allocated blocks in zones of a given type
static void	show_zone_blocks(t_zone *zone, const char *zone_name)
{
	t_block	*block;
	bool	first_zone;

	first_zone = true;
	while (zone)
	{
		// Print zone header only for first zone of this type
		if (first_zone)
		{
			printf("%s : %p\n", zone_name, (void *)zone);
			first_zone = false;
		}
		
		// Print all allocated blocks in this zone
		block = zone->blocks;
		while (block)
		{
			print_block_info(block);
			block = block->next;
		}
		zone = zone->next;
	}
}

// Display all allocated memory (main interface function)
void	show_alloc_mem(void)
{
	size_t	total;

	// Thread-safe memory inspection
	pthread_mutex_lock(&g_memory.mutex);
	
	// Show all zone types
	show_zone_blocks(g_memory.tiny, "TINY");
	show_zone_blocks(g_memory.small, "SMALL");
	show_zone_blocks(g_memory.large, "LARGE");
	
	// Calculate and display total
	total = calculate_total_memory();
	printf("Total : %zu bytes\n", total);
	
	pthread_mutex_unlock(&g_memory.mutex);
}
