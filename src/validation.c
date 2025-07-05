/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouari <ahouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 00:00:00 by ahouari          #+#    #+#             */
/*   Updated: 2025/07/05 18:48:29 by ahouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

// Check if a pointer falls within the memory range of a zone
static bool	is_pointer_in_zone(void *ptr, t_zone *zone)
{
	return (ptr > (void *)zone && ptr < (void *)((char *)zone + zone->size));
}

// Search for a block that exactly matches the given pointer
static bool	find_block_in_zone(void *ptr, t_zone *zone, t_block **block_ptr)
{
	t_block	*block;
	void	*block_data;

	block = zone->blocks;
	while (block)
	{
		block_data = (void *)((char *)block + sizeof(t_block));
		if (ptr == block_data)
		{
			*block_ptr = block;
			return (true);
		}
		block = block->next;
	}
	return (false);
}

// Validate that a pointer was allocated by our malloc
bool	is_valid_pointer(void *ptr, t_block **block_ptr, t_zone **zone_ptr)
{
	t_zone		*zone;
	t_zone_type	type;

	type = TINY;
	while (type <= LARGE)
	{
		zone = get_zone_list(type);
		while (zone)
		{
			if (is_pointer_in_zone(ptr, zone))
			{
				if (find_block_in_zone(ptr, zone, block_ptr))
				{
					*zone_ptr = zone;
					return (true);
				}
			}
			zone = zone->next;
		}
		type++;
	}
	return (false);
}
