/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_helpers2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouari <ahouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 00:00:00 by ahouari          #+#    #+#             */
/*   Updated: 2025/07/05 18:48:30 by ahouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>
#include <unistd.h>

#include "../includes/malloc.h"

// Clean up large zones that are completely free
void	cleanup_large_zone(t_zone *zone)
{
	t_zone	*prev;

	if (zone->type == LARGE && zone->blocks->free && zone->blocks->next == NULL)
	{
		if (g_memory.large == zone)
			g_memory.large = zone->next;
		else
		{
			prev = g_memory.large;
			while (prev && prev->next != zone)
				prev = prev->next;
			if (prev)
				prev->next = zone->next;
		}
		munmap(zone, zone->size);
	}
}

// Get the head of a zone list for the given type
t_zone	*get_zone_list(t_zone_type type)
{
	if (type == TINY)
		return (g_memory.tiny);
	else if (type == SMALL)
		return (g_memory.small);
	else
		return (g_memory.large);
}

// Get system page size
size_t	get_page_size(void)
{
	return (getpagesize());
}
