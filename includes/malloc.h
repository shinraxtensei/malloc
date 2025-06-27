#ifndef MALLOC_H
#define MALLOC_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef __APPLE__
#define GET_PAGE_SIZE() getpagesize()
#else
#define GET_PAGE_SIZE() sysconf(_SC_PAGESIZE)
#endif

#define MALLOC_DEBUG 1
#define MALLOC_SCRIBBLE 2
#define MALLOC_STATISTICS 3

/*
** Memory zones constants - sizes are chosen to balance
** system calls vs memory waste
** According to specification: zones must be multiples of page size
** and contain at least 100 allocations
*/
#define TINY_MAX 128    // Max size for TINY allocations (1 to n bytes)
#define SMALL_MAX 1024  // Max size for SMALL allocations (n+1 to m bytes)
#define ALIGN_SIZE 16   // Alignment boundary

/*
** Zone sizes calculation - must be multiple of page size
** and hold at least 100 allocations of max size plus metadata
*/
#define MIN_ALLOCS_PER_ZONE 100
#define TINY_ZONE_ALLOCS (MIN_ALLOCS_PER_ZONE)
#define SMALL_ZONE_ALLOCS (MIN_ALLOCS_PER_ZONE)

// These will be calculated at runtime based on page size
extern size_t g_tiny_zone_size;
extern size_t g_small_zone_size;

/*
** Memory alignment macros
*/
#define ALIGN(size) (((size) + (ALIGN_SIZE - 1)) & ~(ALIGN_SIZE - 1))

/*
** Zone types
*/
typedef enum { TINY, SMALL, LARGE } t_zone_type;

/*
** Memory block structure
** Used to keep track of allocations
*/
typedef struct s_block {
  size_t size;           // Size of the data area
  bool free;             // Whether the block is free
  struct s_block *next;  // Next block in zone
  struct s_block *prev;  // Previous block in zone
} t_block;

/*
** Zone structure
** Used to manage a memory zone (TINY or SMALL)
*/
typedef struct s_zone {
  t_zone_type type;        // Type of zone
  size_t size;             // Total size of the zone
  struct s_block *blocks;  // First block in the zone
  struct s_zone *next;     // Next zone of the same type
} t_zone;

/*
** Global memory manager structure
*/
typedef struct s_memory {
  t_zone *tiny;           // List of TINY zones
  t_zone *small;          // List of SMALL zones
  t_zone *large;          // List of LARGE zones
  pthread_mutex_t mutex;  // Mutex for thread safety
} t_memory;

/*
** Public function prototypes - matching exact specification
*/
void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);

/*
** Additional mandatory functions
*/
void show_alloc_mem(void);
t_zone *get_zone_list(t_zone_type type);
/*
** Internal function prototypes
*/

t_zone *create_zone(size_t size, t_zone_type type);
void add_zone(t_zone *zone, t_zone_type type);
t_zone_type get_zone_type(size_t size);
t_zone *get_zone_list(t_zone_type type);
t_block *find_free_block(t_zone *zone, size_t size);
t_block *split_block(t_block *block, size_t size);
void coalesce_blocks(t_zone *zone, t_block *block);
bool is_valid_pointer(void *ptr, t_block **block, t_zone **zone);

// Make the global memory manager accessible
extern t_memory g_memory;

#endif /* MALLOC_H */
