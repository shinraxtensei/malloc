/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouari <ahouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 12:00:00 by ahouari          #+#    #+#             */
/*   Updated: 2025/07/05 18:48:25 by ahouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "../includes/malloc.h"

// Print hexadecimal dump of memory content (limited to first 32 bytes)
static void	print_hex_dump(void *start, size_t block_size)
{
	size_t	dump_size;
	size_t	i;

	dump_size = block_size;
	if (dump_size > 32)
		dump_size = 32;
	printf(" [");
	i = 0;
	while (i < dump_size)
	{
		printf("%02x", ((unsigned char *)start)[i]);
		if (i < dump_size - 1)
			printf(" ");
		i++;
	}
	if (block_size > 32)
		printf("...");
	printf("]\n");
}

// Print detailed information about all blocks in a zone with hex dumps
void	print_zone_blocks(t_zone *zone)
{
	t_block	*block;
	void	*start;
	void	*end;

	printf("  Zone %p (size: %zu bytes)\n", (void *)zone, zone->size);
	block = zone->blocks;
	while (block)
	{
		if (!block->free)
		{
			start = (void *)((char *)block + sizeof(t_block));
			end = (void *)((char *)start + block->size - 1);
			printf("    Block %p - %p: %zu bytes", start, end, block->size);
			print_hex_dump(start, block->size);
		}
		block = block->next;
	}
}
