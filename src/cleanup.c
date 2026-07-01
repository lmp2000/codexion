/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	destroy_coders(t_sim *sim)
{
	int	i;

	if (sim->coders)
	{
		i = 0;
		while (i < sim->config.number_of_coders)
		{
			if (sim->coders[i].mutex_ready)
				pthread_mutex_destroy(&sim->coders[i].mutex);
			i++;
		}
		free(sim->coders);
		sim->coders = NULL;
	}
}

void	destroy_sim(t_sim *sim)
{
	heap_destroy(&sim->scheduler);
	destroy_dongles(sim);
	destroy_coders(sim);
	if (sim->scheduler_cond_ready)
	{
		pthread_cond_destroy(&sim->scheduler_cond);
		sim->scheduler_cond_ready = 0;
	}
	if (sim->scheduler_mutex_ready)
	{
		pthread_mutex_destroy(&sim->scheduler_mutex);
		sim->scheduler_mutex_ready = 0;
	}
	if (sim->state_mutex_ready)
	{
		pthread_mutex_destroy(&sim->state_mutex);
		sim->state_mutex_ready = 0;
	}
	if (sim->log_mutex_ready)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		sim->log_mutex_ready = 0;
	}
}
