/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 00:00:00 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 15:25:01 by anasshouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

static bool	is_pointer_in_zone(void *ptr, t_zone *zone)
{
	return (ptr > (void *)zone && ptr < (void *)((char *)zone + zone->size));
}

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
