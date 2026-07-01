/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->config.number_of_coders);
	if (!sim->coders)
		return (1);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].thread = 0;
		sim->coders[i].last_compile_time = sim->start_time;
		sim->coders[i].compile_count = 0;
		sim->coders[i].mutex_ready = 0;
		sim->coders[i].sim = sim;
		if (pthread_mutex_init(&sim->coders[i].mutex, NULL) != 0)
			return (1);
		sim->coders[i].mutex_ready = 1;
		i++;
	}
	return (0);
}

static int	init_error(t_sim *sim)
{
	destroy_sim(sim);
	return (1);
}

static int	init_sync(t_sim *sim)
{
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (1);
	sim->log_mutex_ready = 1;
	if (pthread_mutex_init(&sim->state_mutex, NULL) != 0)
		return (init_error(sim));
	sim->state_mutex_ready = 1;
	if (pthread_mutex_init(&sim->scheduler_mutex, NULL) != 0)
		return (init_error(sim));
	sim->scheduler_mutex_ready = 1;
	if (pthread_cond_init(&sim->scheduler_cond, NULL) != 0)
		return (init_error(sim));
	sim->scheduler_cond_ready = 1;
	return (0);
}

int	init_sim(t_sim *sim, t_config *config)
{
	sim->config = *config;
	sim->coders = NULL;
	sim->dongles = NULL;
	sim->scheduler.items = NULL;
	sim->scheduler.size = 0;
	sim->scheduler.capacity = 0;
	sim->next_request_seq = 0;
	sim->log_mutex_ready = 0;
	sim->state_mutex_ready = 0;
	sim->scheduler_mutex_ready = 0;
	sim->scheduler_cond_ready = 0;
	sim->stop = 0;
	sim->start_time = get_time_ms();
	if (sim->start_time < 0)
		return (1);
	if (init_sync(sim) != 0)
		return (1);
	if (init_coders(sim) != 0)
		return (init_error(sim));
	if (init_dongles(sim) != 0)
		return (init_error(sim));
	if (heap_init(&sim->scheduler, sim->config.number_of_coders,
			sim->config.scheduler) != 0)
		return (init_error(sim));
	return (0);
}
