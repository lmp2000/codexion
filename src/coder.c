/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	coder_compile(t_coder *coder)
{
	int	first;
	int	second;

	if (scheduler_submit_request(coder) != 0)
		return ;
	if (scheduler_wait_turn(coder) != 0)
		return ;
	if (acquire_dongles(coder, &first, &second) != 0)
	{
		scheduler_complete_request(coder);
		return ;
	}
	update_coder_compile_state(coder);
	log_state(coder->sim, coder->id, "is compiling");
	precise_sleep(coder->sim->config.time_to_compile, coder->sim);
	if (!sim_should_stop(coder->sim))
		complete_coder_compile(coder);
	release_dongles(coder, first, second);
	scheduler_complete_request(coder);
}

static void	coder_debug(t_coder *coder)
{
	log_state(coder->sim, coder->id, "is debugging");
	precise_sleep(coder->sim->config.time_to_debug, coder->sim);
}

static void	coder_refactor(t_coder *coder)
{
	log_state(coder->sim, coder->id, "is refactoring");
	precise_sleep(coder->sim->config.time_to_refactor, coder->sim);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!sim_should_stop(coder->sim)
		&& get_coder_compile_count(coder)
		< coder->sim->config.number_of_compiles_required)
	{
		coder_compile(coder);
		if (sim_should_stop(coder->sim))
			break ;
		coder_debug(coder);
		if (sim_should_stop(coder->sim))
			break ;
		coder_refactor(coder);
	}
	return (NULL);
}
