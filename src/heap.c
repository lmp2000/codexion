#include "codexion.h"

static long	request_priority(t_heap *heap, t_request request)
{
	if (heap->type == CODEXION_SCHED_FIFO)
		return (request.request_time);
	return (request.deadline);
}

static int	request_is_before(t_heap *heap, t_request a, t_request b)
{
	long	priority_a;
	long	priority_b;

	priority_a = request_priority(heap, a);
	priority_b = request_priority(heap, b);
	if (priority_a < priority_b)
		return (1);
	if (priority_a > priority_b)
		return (0);
	return (a.coder_id < b.coder_id);
}

static void	swap_requests(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

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

static void	heap_bubble_up(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!request_is_before(heap, heap->items[index], heap->items[parent]))
			break ;
		swap_requests(&heap->items[index], &heap->items[parent]);
		index = parent;
	}
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

static void	heap_bubble_down(t_heap *heap, int index)
{
	int	left;
	int	right;
	int	best;

	while (1)
	{
		left = (index * 2) + 1;
		right = (index * 2) + 2;
		best = index;
		if (left < heap->size
			&& request_is_before(heap, heap->items[left], heap->items[best]))
			best = left;
		if (right < heap->size
			&& request_is_before(heap, heap->items[right], heap->items[best]))
			best = right;
		if (best == index)
			break ;
		swap_requests(&heap->items[index], &heap->items[best]);
		index = best;
	}
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

int	heap_is_empty(t_heap *heap)
{
	return (heap->size == 0);
}