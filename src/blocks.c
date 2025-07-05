/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blocks.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouari <ahouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 18:46:00 by ahouari       #+#    #+#             */
/*   Updated: 2025/07/04 18:46:00 by ahouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>

#include "../includes/malloc.h"

t_block	*find_free_block(t_zone *zone, size_t size)
{
	t_block	*block;

	block = zone->blocks;
	while (block)
	{
		if (block->free && block->size >= size)
			return (block);
		block = block->next;
	}
	return (NULL);
}

t_block	*split_block(t_block *block, size_t size)
{
	t_block	*new_block;
	size_t	remaining_size;

	if (block->size < size + sizeof(t_block) + ALIGN_SIZE)
		return (block);
	remaining_size = block->size - size - sizeof(t_block);
	new_block = (t_block *)((char *)block + sizeof(t_block) + size);
	new_block->size = remaining_size;
	new_block->free = true;
	new_block->next = block->next;
	new_block->prev = block;
	if (block->next)
		block->next->prev = new_block;
	block->next = new_block;
	block->size = size;
	return (block);
}

void	coalesce_blocks(t_zone *zone, t_block *block)
{
	t_block	*next_block;

	(void)zone;
	if (!block->free)
		return ;
	next_block = block->next;
	while (next_block && next_block->free)
	{
		block->size += next_block->size + sizeof(t_block);
		block->next = next_block->next;
		if (next_block->next)
			next_block->next->prev = block;
		next_block = next_block->next;
	}
}
