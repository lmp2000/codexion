/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_grant.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	assign_dongles(t_coder *coder, int left, int right)
{
	coder->sim->dongles[left].available = 0;
	coder->sim->dongles[left].owner_id = coder->id;
	coder->sim->dongles[right].available = 0;
	coder->sim->dongles[right].owner_id = coder->id;
}

static int	coder_dongles_ready(t_sim *sim, int coder_id)
{
	t_dongle	*locks[2];
	int			index[2];
	int			ready;
	long		now;

	index[0] = coder_id - 1;
	index[1] = coder_id % sim->config.number_of_coders;
	if (index[0] == index[1])
		return (0);
	now = get_time_ms();
	scheduler_get_dongle_order(sim, index, locks);
	pthread_mutex_lock(&locks[0]->mutex);
	pthread_mutex_lock(&locks[1]->mutex);
	ready = (sim->dongles[index[0]].available
			&& sim->dongles[index[1]].available
			&& now >= sim->dongles[index[0]].cooldown_until
			&& now >= sim->dongles[index[1]].cooldown_until);
	pthread_mutex_unlock(&locks[1]->mutex);
	pthread_mutex_unlock(&locks[0]->mutex);
	return (ready);
}

static int	higher_ready_conflict(t_coder *coder, t_request request)
{
	t_request	other;
	int			i;

	i = 0;
	while (i < coder->sim->scheduler.size)
	{
		other = coder->sim->scheduler.items[i];
		if (other.coder_id != request.coder_id
			&& scheduler_request_before(coder->sim, other, request)
			&& scheduler_share_dongle(coder->sim, other.coder_id,
				request.coder_id)
			&& coder_dongles_ready(coder->sim, other.coder_id))
			return (1);
		i++;
	}
	return (0);
}

static int	grant_dongles(t_coder *coder, int left, int right)
{
	t_dongle	*locks[2];
	int			index[2];
	long		now;

	now = get_time_ms();
	index[0] = left;
	index[1] = right;
	scheduler_get_dongle_order(coder->sim, index, locks);
	pthread_mutex_lock(&locks[0]->mutex);
	pthread_mutex_lock(&locks[1]->mutex);
	if (coder->sim->dongles[left].available
		&& coder->sim->dongles[right].available
		&& now >= coder->sim->dongles[left].cooldown_until
		&& now >= coder->sim->dongles[right].cooldown_until)
	{
		assign_dongles(coder, left, right);
		now = 1;
	}
	else
		now = 0;
	pthread_mutex_unlock(&locks[1]->mutex);
	pthread_mutex_unlock(&locks[0]->mutex);
	return ((int)now);
}

int	scheduler_can_grant(t_coder *coder, int left, int right)
{
	t_request	request;

	if (scheduler_find_request(coder->sim, coder->id, &request) != 0)
		return (0);
	if (higher_ready_conflict(coder, request))
		return (0);
	return (grant_dongles(coder, left, right));
}
