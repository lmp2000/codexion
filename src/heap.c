/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_init(t_heap *heap, int capacity, t_scheduler type)
{
	heap->items = malloc(sizeof(t_request) * capacity);
	if (!heap->items)
		return (1);
	heap->size = 0;
	heap->capacity = capacity;
	heap->type = type;
	return (0);
}

void	heap_destroy(t_heap *heap)
{
	if (heap->items)
		free(heap->items);
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

int	heap_push(t_heap *heap, t_request request)
{
	if (heap->size >= heap->capacity)
		return (1);
	heap->items[heap->size] = request;
	heap_bubble_up(heap, heap->size);
	heap->size++;
	return (0);
}

int	heap_pop(t_heap *heap, t_request *out)
{
	if (heap->size == 0)
		return (1);
	*out = heap->items[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		heap_bubble_down(heap, 0);
	}
	return (0);
}

int	heap_peek(t_heap *heap, t_request *out)
{
	if (heap->size == 0)
		return (1);
	*out = heap->items[0];
	return (0);
}
