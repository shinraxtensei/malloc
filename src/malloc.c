/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 00:00:00 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 17:24:12 by anasshouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

size_t		g_tiny_zone_size = 0;
size_t		g_small_zone_size = 0;

t_memory	g_memory = {
	.tiny = NULL,
	.small = NULL,
	.large = NULL,
	.mutex = PTHREAD_MUTEX_INITIALIZER
};

static bool	g_initialized = false;

static void	calculate_zone_sizes(void)
{
	size_t	page_size;
	size_t	tiny_needed;
	size_t	small_needed;

	if (g_tiny_zone_size != 0)
		return ;
	page_size = get_page_size();
	tiny_needed = (TINY_MAX + sizeof(t_block)) * TINY_ZONE_ALLOCS;
	small_needed = (SMALL_MAX + sizeof(t_block)) * SMALL_ZONE_ALLOCS;
	g_tiny_zone_size = ((tiny_needed + page_size - 1) / page_size)
		* page_size;
	g_small_zone_size = ((small_needed + page_size - 1) / page_size)
		* page_size;
}

void	init_memory_manager(void)
{
	if (!g_initialized)
	{
		calculate_zone_sizes();
		pthread_mutex_init(&g_memory.mutex, NULL);
		g_initialized = true;
	}
}

void	*malloc(size_t size)
{
	void	*ptr;
	size_t	aligned_size;

	if (size == 0)
		return (NULL);
	init_memory_manager();
	aligned_size = align_size(size);
	pthread_mutex_lock(&g_memory.mutex);
	ptr = allocate_memory(aligned_size);
	pthread_mutex_unlock(&g_memory.mutex);
	return (ptr);
}

void	free(void *ptr)
{
	if (ptr == NULL)
		return ;
	init_memory_manager();
	pthread_mutex_lock(&g_memory.mutex);
	free_memory(ptr);
	pthread_mutex_unlock(&g_memory.mutex);
}
