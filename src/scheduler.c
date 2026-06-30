#include "codexion.h"

static t_request	create_request(t_coder *coder)
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

static int	request_before(t_sim *sim, t_request a, t_request b)
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

static int	find_request(t_sim *sim, int coder_id, t_request *out)
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

static int	share_dongle(t_sim *sim, int first_coder, int second_coder)
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

static int	coder_dongles_ready(t_sim *sim, int coder_id)
{
	t_dongle	*first;
	t_dongle	*second;
	int			left;
	int			right;
	int			ready;
	long		now;

	left = coder_id - 1;
	right = coder_id % sim->config.number_of_coders;
	if (left == right)
		return (0);
	now = get_time_ms();
	if (left < right)
	{
		first = &sim->dongles[left];
		second = &sim->dongles[right];
	}
	else
	{
		first = &sim->dongles[right];
		second = &sim->dongles[left];
	}
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	ready = (sim->dongles[left].available && sim->dongles[right].available
			&& now >= sim->dongles[left].cooldown_until
			&& now >= sim->dongles[right].cooldown_until);
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
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
		if (other.coder_id != request.coder_id && request_before(coder->sim,
				other, request) && share_dongle(coder->sim, other.coder_id,
				request.coder_id) && coder_dongles_ready(coder->sim,
				other.coder_id))
			return (1);
		i++;
	}
	return (0);
}

static int	grant_dongles(t_coder *coder, int left, int right)
{
	t_dongle	*first;
	t_dongle	*second;
	long		now;

	now = get_time_ms();
	if (left < right)
	{
		first = &coder->sim->dongles[left];
		second = &coder->sim->dongles[right];
	}
	else
	{
		first = &coder->sim->dongles[right];
		second = &coder->sim->dongles[left];
	}
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	if (coder->sim->dongles[left].available
		&& coder->sim->dongles[right].available
		&& now >= coder->sim->dongles[left].cooldown_until
		&& now >= coder->sim->dongles[right].cooldown_until)
	{
		coder->sim->dongles[left].available = 0;
		coder->sim->dongles[left].owner_id = coder->id;
		coder->sim->dongles[right].available = 0;
		coder->sim->dongles[right].owner_id = coder->id;
		pthread_mutex_unlock(&second->mutex);
		pthread_mutex_unlock(&first->mutex);
		return (1);
	}
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (0);
}

static int	can_grant(t_coder *coder, int left, int right)
{
	t_request	request;

	if (find_request(coder->sim, coder->id, &request) != 0)
		return (0);
	if (higher_ready_conflict(coder, request))
		return (0);
	return (grant_dongles(coder, left, right));
}

int	scheduler_submit_request(t_coder *coder)
{
	t_request	request;
	int			result;

	pthread_mutex_lock(&coder->sim->scheduler_mutex);
	request = create_request(coder);
	request.seq = coder->sim->next_request_seq;
	coder->sim->next_request_seq++;
	result = heap_push(&coder->sim->scheduler, request);
	pthread_cond_broadcast(&coder->sim->scheduler_cond);
	pthread_mutex_unlock(&coder->sim->scheduler_mutex);
	return (result);
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
		if (find_request(coder->sim, coder->id, &request) == 0)
		{
			if (left == right || can_grant(coder, left, right))
			{
				heap_remove_coder(&coder->sim->scheduler, coder->id, &request);
				pthread_cond_broadcast(&coder->sim->scheduler_cond);
				pthread_mutex_unlock(&coder->sim->scheduler_mutex);
				return (0);
			}
		}
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