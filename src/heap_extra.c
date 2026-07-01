/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_extra.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	rebuild_heap(t_heap *heap)
{
	int	child;
	int	parent;
	int	i;

	i = 1;
	while (i < heap->size)
	{
		child = i;
		while (child > 0)
		{
			parent = (child - 1) / 2;
			if (!heap_request_before(heap, heap->items[child],
					heap->items[parent]))
				break ;
			heap_swap_requests(&heap->items[child], &heap->items[parent]);
			child = parent;
		}
		i++;
	}
}

int	heap_is_empty(t_heap *heap)
{
	return (heap->size == 0);
}

int	heap_remove_coder(t_heap *heap, int coder_id, t_request *out)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->items[i].coder_id == coder_id)
		{
			if (out)
				*out = heap->items[i];
			heap->size--;
			if (i < heap->size)
				heap->items[i] = heap->items[heap->size];
			rebuild_heap(heap);
			return (0);
		}
		i++;
	}
	return (1);
}
