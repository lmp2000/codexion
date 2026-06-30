#include "codexion.h"

static int	create_coder_threads(t_sim *sim, int *created_count)
{
	int	i;

	*created_count = 0;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
		{
			set_sim_stop(sim);
			pthread_mutex_lock(&sim->scheduler_mutex);
			pthread_cond_broadcast(&sim->scheduler_cond);
			pthread_mutex_unlock(&sim->scheduler_mutex);
			return (1);
		}
		(*created_count)++;
		i++;
	}
	return (0);
}

static void	join_coder_threads(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

int	start_simulation(t_sim *sim)
{
	int created_count;

	if (create_coder_threads(sim, &created_count) != 0)
	{
		join_coder_threads(sim, created_count);
		return (1);
	}
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
	{
		set_sim_stop(sim);
		pthread_mutex_lock(&sim->scheduler_mutex);
		pthread_cond_broadcast(&sim->scheduler_cond);
		pthread_mutex_unlock(&sim->scheduler_mutex);
		join_coder_threads(sim, created_count);
		return (1);
	}
	join_coder_threads(sim, created_count);
	pthread_join(sim->monitor_thread, NULL);
	return (0);
}