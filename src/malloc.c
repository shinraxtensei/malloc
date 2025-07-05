/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 00:00:00 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 18:48:27 by anasshouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc.h"

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

// Global zone size variables (calculated at runtime)
size_t		g_tiny_zone_size = 0;
size_t		g_small_zone_size = 0;

// Global memory manager instance
t_memory	g_memory = {
	.tiny = NULL,
	.small = NULL,
	.large = NULL,
	.mutex = PTHREAD_MUTEX_INITIALIZER
};

// Initialization flag to ensure one-time setup
static bool	g_initialized = false;

// Calculate optimal zone sizes based on page size
static void	calculate_zone_sizes(void)
{
	size_t	page_size;
	size_t	tiny_needed;
	size_t	small_needed;

	// Skip if already calculated
	if (g_tiny_zone_size != 0)
		return ;
	
	// Get system page size and calculate space needed
	page_size = get_page_size();
	tiny_needed = (TINY_MAX + sizeof(t_block)) * TINY_ZONE_ALLOCS;
	small_needed = (SMALL_MAX + sizeof(t_block)) * SMALL_ZONE_ALLOCS;
	
	// Round up to page boundaries for memory efficiency
	g_tiny_zone_size = ((tiny_needed + page_size - 1) / page_size)
		* page_size;
	g_small_zone_size = ((small_needed + page_size - 1) / page_size)
		* page_size;
}

// One-time initialization of the memory manager
void	init_memory_manager(void)
{
	// Only initialize once
	if (!g_initialized)
	{
		calculate_zone_sizes();
		pthread_mutex_init(&g_memory.mutex, NULL);
		g_initialized = true;
	}
}

// Main memory allocation function
void	*malloc(size_t size)
{
	void	*ptr;
	size_t	aligned_size;

	// Handle edge case: zero allocation
	if (size == 0)
		return (NULL);
	
	// Ensure system is initialized
	init_memory_manager();
	
	// Round size up to alignment boundary
	aligned_size = align_size(size);
	
	// Thread-safe allocation
	pthread_mutex_lock(&g_memory.mutex);
	ptr = allocate_memory(aligned_size);
	pthread_mutex_unlock(&g_memory.mutex);
	
	return (ptr);
}

// Free previously allocated memory
void	free(void *ptr)
{
	// Handle edge case: NULL pointer
	if (ptr == NULL)
		return ;
	
	// Ensure system is initialized
	init_memory_manager();
	
	// Thread-safe deallocation
	pthread_mutex_lock(&g_memory.mutex);
	free_memory(ptr);
	pthread_mutex_unlock(&g_memory.mutex);
}
