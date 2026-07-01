/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	scheduler_submit_request(t_coder *coder)
{
	t_request	request;
	int			result;

	pthread_mutex_lock(&coder->sim->scheduler_mutex);
	request = scheduler_create_request(coder);
	request.seq = coder->sim->next_request_seq;
	coder->sim->next_request_seq++;
	result = heap_push(&coder->sim->scheduler, request);
	pthread_cond_broadcast(&coder->sim->scheduler_cond);
	pthread_mutex_unlock(&coder->sim->scheduler_mutex);
	return (result);
}

static int	finish_turn(t_coder *coder, t_request *request)
{
	heap_remove_coder(&coder->sim->scheduler, coder->id, request);
	pthread_cond_broadcast(&coder->sim->scheduler_cond);
	pthread_mutex_unlock(&coder->sim->scheduler_mutex);
	return (0);
}

int	scheduler_wait_turn(t_coder *coder)
{
	t_request	request;
	int			left;
	int			right;

	left = coder->id - 1;
	right = coder->id % coder->sim->config.number_of_coders;
	pthread_mutex_lock(&coder->sim->scheduler_mutex);
	while (!sim_should_stop(coder->sim))
	{
		if (scheduler_find_request(coder->sim, coder->id, &request) == 0
			&& (left == right || scheduler_can_grant(coder, left, right)))
			return (finish_turn(coder, &request));
		pthread_mutex_unlock(&coder->sim->scheduler_mutex);
		usleep(500);
		pthread_mutex_lock(&coder->sim->scheduler_mutex);
	}
	pthread_mutex_unlock(&coder->sim->scheduler_mutex);
	return (1);
}

void	scheduler_complete_request(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->scheduler_mutex);
	pthread_cond_broadcast(&coder->sim->scheduler_cond);
	pthread_mutex_unlock(&coder->sim->scheduler_mutex);
}
