#include "codexion.h"

static int	all_coders_completed(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (get_coder_compile_count(&sim->coders[i]) < sim->config.number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

static int	check_burnout(t_sim *sim)
{
	int		i;
	long	now;
	long	last_compile_time;

	i = 0;
	now = get_time_ms();
	while (i < sim->config.number_of_coders)
	{
		last_compile_time = get_coder_last_compile_time(&sim->coders[i]);
		if (now - last_compile_time >= sim->config.time_to_burnout)
		{
			log_burnout(sim, sim->coders[i].id);
			return (1);
		}
		i++;
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_sim *sim;

	sim = (t_sim *)arg;
	while (!sim_should_stop(sim))
	{
		if (check_burnout(sim))
			return (NULL);
		if (all_coders_completed(sim))
		{
			set_sim_stop(sim);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}