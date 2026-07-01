/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_order.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long	request_priority(t_heap *heap, t_request request)
{
	if (heap->type == CODEXION_SCHED_FIFO)
		return (request.request_time);
	return (request.deadline);
}

int	heap_request_before(t_heap *heap, t_request a, t_request b)
{
	long	priority_a;
	long	priority_b;

	if (heap->type == CODEXION_SCHED_FIFO)
		return (a.seq < b.seq);
	priority_a = request_priority(heap, a);
	priority_b = request_priority(heap, b);
	if (priority_a < priority_b)
		return (1);
	if (priority_a > priority_b)
		return (0);
	if (a.seq < b.seq)
		return (1);
	if (a.seq > b.seq)
		return (0);
	return (a.coder_id < b.coder_id);
}

void	heap_swap_requests(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heap_bubble_up(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!heap_request_before(heap, heap->items[index],
				heap->items[parent]))
			break ;
		heap_swap_requests(&heap->items[index], &heap->items[parent]);
		index = parent;
	}
}

void	heap_bubble_down(t_heap *heap, int index)
{
	int	left;
	int	right;
	int	best;

	while (1)
	{
		left = (index * 2) + 1;
		right = (index * 2) + 2;
		best = index;
		if (left < heap->size && heap_request_before(heap, heap->items[left],
				heap->items[best]))
			best = left;
		if (right < heap->size && heap_request_before(heap, heap->items[right],
				heap->items[best]))
			best = right;
		if (best == index)
			break ;
		heap_swap_requests(&heap->items[index], &heap->items[best]);
		index = best;
	}
}
