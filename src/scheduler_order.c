/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_order.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_request	scheduler_create_request(t_coder *coder)
{
	t_request	request;
	long		now;

	now = get_time_ms();
	request.coder_id = coder->id;
	request.request_time = now;
	request.deadline = get_coder_last_compile_time(coder)
		+ coder->sim->config.time_to_burnout;
	return (request);
}

int	scheduler_request_before(t_sim *sim, t_request a, t_request b)
{
	if (sim->config.scheduler == CODEXION_SCHED_FIFO)
		return (a.seq < b.seq);
	if (a.deadline < b.deadline)
		return (1);
	if (a.deadline > b.deadline)
		return (0);
	if (a.seq < b.seq)
		return (1);
	if (a.seq > b.seq)
		return (0);
	return (a.coder_id < b.coder_id);
}

int	scheduler_find_request(t_sim *sim, int coder_id, t_request *out)
{
	int	i;

	i = 0;
	while (i < sim->scheduler.size)
	{
		if (sim->scheduler.items[i].coder_id == coder_id)
		{
			*out = sim->scheduler.items[i];
			return (0);
		}
		i++;
	}
	return (1);
}

int	scheduler_share_dongle(t_sim *sim, int first_coder, int second_coder)
{
	int	first_left;
	int	first_right;
	int	second_left;
	int	second_right;

	first_left = first_coder - 1;
	first_right = first_coder % sim->config.number_of_coders;
	second_left = second_coder - 1;
	second_right = second_coder % sim->config.number_of_coders;
	return (first_left == second_left || first_left == second_right
		|| first_right == second_left || first_right == second_right);
}

void	scheduler_get_dongle_order(t_sim *sim, int *index, t_dongle **locks)
{
	if (index[0] < index[1])
	{
		locks[0] = &sim->dongles[index[0]];
		locks[1] = &sim->dongles[index[1]];
	}
	else
	{
		locks[0] = &sim->dongles[index[1]];
		locks[1] = &sim->dongles[index[0]];
	}
}
