/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_access.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	try_take_dongle(t_coder *coder, int index)
{
	t_dongle	*dongle;
	long		now;

	dongle = &coder->sim->dongles[index];
	now = get_time_ms();
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->available && now >= dongle->cooldown_until)
	{
		dongle->available = 0;
		dongle->owner_id = coder->id;
		pthread_mutex_unlock(&dongle->mutex);
		return (1);
	}
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

static void	unlock_taken_dongle(t_coder *coder, int index)
{
	t_dongle	*dongle;
	long		now;

	dongle = &coder->sim->dongles[index];
	now = get_time_ms();
	pthread_mutex_lock(&dongle->mutex);
	dongle->available = 1;
	dongle->owner_id = 0;
	dongle->cooldown_until = now + coder->sim->config.dongle_cooldown;
	pthread_mutex_unlock(&dongle->mutex);
}

static int	acquire_single_dongle(t_coder *coder, int *first, int left)
{
	if (try_take_dongle(coder, left))
	{
		*first = left;
		log_state(coder->sim, coder->id, "has taken a dongle");
	}
	while (!sim_should_stop(coder->sim))
		usleep(500);
	if (*first != -1)
	{
		unlock_taken_dongle(coder, *first);
		*first = -1;
	}
	return (1);
}

int	acquire_dongles(t_coder *coder, int *first, int *second)
{
	int	left;
	int	right;

	*first = -1;
	*second = -1;
	left = coder->id - 1;
	right = coder->id % coder->sim->config.number_of_coders;
	if (left == right)
		return (acquire_single_dongle(coder, first, left));
	*first = left;
	*second = right;
	log_state(coder->sim, coder->id, "has taken a dongle");
	log_state(coder->sim, coder->id, "has taken a dongle");
	return (0);
}

void	release_dongles(t_coder *coder, int first, int second)
{
	pthread_mutex_lock(&coder->sim->scheduler_mutex);
	if (first >= 0)
		unlock_taken_dongle(coder, first);
	if (second >= 0)
		unlock_taken_dongle(coder, second);
	pthread_cond_broadcast(&coder->sim->scheduler_cond);
	pthread_mutex_unlock(&coder->sim->scheduler_mutex);
}
