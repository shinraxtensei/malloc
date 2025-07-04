/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouari <ahouari@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 18:45:15 by ahouari       #+#    #+#             */
/*   Updated: 2025/07/04 18:45:15 by ahouari      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

#include "../includes/malloc.h"

/**
 * Custom memset implementation for internal use
 */
void *ft_memset(void *s, int c, size_t n) {
  unsigned char *ptr = (unsigned char *)s;
  unsigned char value = (unsigned char)c;

  while (n--) {
    *ptr++ = value;
  }
  return s;
}

/**
 * Custom memcpy implementation for internal use
 */
void *ft_memcpy(void *dest, const void *src, size_t n) {
  unsigned char *d = (unsigned char *)dest;
  const unsigned char *s = (const unsigned char *)src;

  if (dest == src || n == 0) {
    return dest;
  }

  while (n--) {
    *d++ = *s++;
  }
  return dest;
}

/**
 * Get zone list by type (centralized function)
 */
t_zone *get_zone_list(t_zone_type type) {
  if (type == TINY)
    return g_memory.tiny;
  else if (type == SMALL)
    return g_memory.small;
  else
    return g_memory.large;
}
