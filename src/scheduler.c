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

int	scheduler_submit_request(t_coder *coder)
{
	t_request	request;
	int			result;

	request = create_request(coder);
	pthread_mutex_lock(&coder->sim->scheduler_mutex);
	result = heap_push(&coder->sim->scheduler, request);
	pthread_cond_broadcast(&coder->sim->scheduler_cond);
	pthread_mutex_unlock(&coder->sim->scheduler_mutex);
	return (result);
}

int	scheduler_wait_turn(t_coder *coder)
{
	t_request	top;

	pthread_mutex_lock(&coder->sim->scheduler_mutex);
	while (!sim_should_stop(coder->sim))
	{
		if (heap_peek(&coder->sim->scheduler, &top) == 0
			&& top.coder_id == coder->id)
		{
			heap_pop(&coder->sim->scheduler, &top);
			pthread_cond_broadcast(&coder->sim->scheduler_cond);
			pthread_mutex_unlock(&coder->sim->scheduler_mutex);
			return (0);
		}
		pthread_cond_wait(&coder->sim->scheduler_cond,
			&coder->sim->scheduler_mutex);
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