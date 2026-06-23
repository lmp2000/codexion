#include "codexion.h"

static int	create_coder_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			set_sim_stop(sim);
			return (1);
		}
		i++;
	}
	return (0);
}

static void	join_coder_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

int	start_simulation(t_sim *sim)
{
	if (create_coder_threads(sim) != 0)
		return (1);
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
	{
		set_sim_stop(sim);
		join_coder_threads(sim);
		return (1);
	}
	join_coder_threads(sim);
	pthread_join(sim->monitor_thread, NULL);
	return (0);
}