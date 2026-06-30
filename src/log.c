#include "codexion.h"

void	log_state(t_sim *sim, int coder_id, char *message)
{
	long	timestamp;
	int		stopped;

	pthread_mutex_lock(&sim->log_mutex);
	pthread_mutex_lock(&sim->state_mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->state_mutex);
	if (stopped)
	{
		pthread_mutex_unlock(&sim->log_mutex);
		return ;
	}
	timestamp = get_time_ms() - sim->start_time;
	printf("%ld %d %s\n", timestamp, coder_id, message);
	pthread_mutex_unlock(&sim->log_mutex);
}

int	log_burnout(t_sim *sim, int coder_id)
{
	long timestamp;

	pthread_mutex_lock(&sim->log_mutex);
	pthread_mutex_lock(&sim->state_mutex);
	if (sim->stop)
	{
		pthread_mutex_unlock(&sim->state_mutex);
		pthread_mutex_unlock(&sim->log_mutex);
		return (0);
	}
	sim->stop = 1;
	timestamp = get_time_ms() - sim->start_time;
	printf("%ld %d burned out\n", timestamp, coder_id);
	pthread_mutex_unlock(&sim->state_mutex);
	pthread_mutex_unlock(&sim->log_mutex);
	pthread_cond_broadcast(&sim->scheduler_cond);
	return (1);
}