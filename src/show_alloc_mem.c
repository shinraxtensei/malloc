/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 18:45:24 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 15:25:01 by anasshouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "../includes/malloc.h"

static void	print_block_info(t_block *block)
{
	void	*start;
	void	*end;

	if (!block->free)
	{
		start = (void *)((char *)block + sizeof(t_block));
		end = (void *)((char *)start + block->size - 1);
		printf("%p - %p : %zu bytes\n", start, end, block->size);
	}
}

static void	show_zone_blocks(t_zone *zone, const char *zone_name)
{
	t_block	*block;
	bool	first_zone;

	first_zone = true;
	while (zone)
	{
		if (first_zone)
		{
			printf("%s : %p\n", zone_name, (void *)zone);
			first_zone = false;
		}
		block = zone->blocks;
		while (block)
		{
			print_block_info(block);
			block = block->next;
		}
		zone = zone->next;
	}
}

void	show_alloc_mem(void)
{
	size_t	total;

	pthread_mutex_lock(&g_memory.mutex);
	show_zone_blocks(g_memory.tiny, "TINY");
	show_zone_blocks(g_memory.small, "SMALL");
	show_zone_blocks(g_memory.large, "LARGE");
	total = calculate_total_memory();
	printf("Total : %zu bytes\n", total);
	pthread_mutex_unlock(&g_memory.mutex);
}
