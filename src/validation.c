/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 00:00:00 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 18:48:29 by anasshouari      ###   ########.fr       */
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
		// Calculate where user data starts (after block metadata)
		block_data = (void *)((char *)block + sizeof(t_block));
		
		// Check for exact match
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

	// Search through all zone types
	type = TINY;
	while (type <= LARGE)
	{
		// Get the first zone of this type
		zone = get_zone_list(type);
		
		// Check all zones of this type
		while (zone)
		{
			// First check if pointer is in zone's memory range
			if (is_pointer_in_zone(ptr, zone))
			{
				// Then check if it exactly matches a block's data start
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
