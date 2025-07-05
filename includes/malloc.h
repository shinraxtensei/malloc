/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anasshouari <anasshouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/05 00:00:00 by ashouari          #+#    #+#             */
/*   Updated: 2025/07/05 18:59:11 by anasshouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
# define MALLOC_H

# include <pthread.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdlib.h>
# include <sys/mman.h>
# include <unistd.h>

# define TINY_MAX 128
# define SMALL_MAX 1024
# define ALIGN_SIZE 16

# define MIN_ALLOCS_PER_ZONE 100
# define TINY_ZONE_ALLOCS 100
# define SMALL_ZONE_ALLOCS 100



typedef enum e_zone_type
{
	TINY,
	SMALL,
	LARGE
}	t_zone_type;

typedef struct s_block
{
	size_t			size;
	bool			free;
	struct s_block	*next;
	struct s_block	*prev;
}	t_block;

typedef struct s_zone
{
	t_zone_type		type;
	size_t			size;
	struct s_block	*blocks;
	struct s_zone	*next;
}	t_zone;

typedef struct s_memory
{
	t_zone			*tiny;
	t_zone			*small;
	t_zone			*large;
	size_t			tiny_zone_size;
	size_t			small_zone_size;
}	t_memory;

// External declarations for global variables (limited to 2 by subject)
extern t_memory			g_memory;
extern pthread_mutex_t	g_malloc_mutex;

size_t	ft_strlen(const char *s);
void	*ft_memset(void *s, int c, size_t n);
void	*ft_memcpy(void *dest, const void *src, size_t n);

void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);
void	show_alloc_mem(void);
void	show_alloc_mem_ex(void);
void	print_zone_blocks(t_zone *zone);
void	init_memory_manager(void);
void	*allocate_memory(size_t aligned_size);
void	free_memory(void *ptr);

t_zone		*create_zone(size_t size, t_zone_type type);
void		add_zone(t_zone *zone, t_zone_type type);
t_zone_type	get_zone_type(size_t size);
t_zone		*get_zone_list(t_zone_type type);
t_block		*find_free_block(t_zone *zone, size_t size);
t_block		*split_block(t_block *block, size_t size);
void		coalesce_blocks(t_zone *zone, t_block *block);
bool		is_valid_pointer(void *ptr, t_block **block, t_zone **zone);
void		cleanup_large_zone(t_zone *zone);
size_t		calculate_total_memory(void);

size_t		align_size(size_t size);
size_t		get_page_size(void);

extern t_memory	g_memory;

#endif
