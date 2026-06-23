#include "codexion.h"

static int	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->config.number_of_coders);
	if (!sim->coders)
		return (1);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].last_compile_time = sim->start_time;
		sim->coders[i].compile_count = 0;
		sim->coders[i].sim = sim;
		if (pthread_mutex_init(&sim->coders[i].mutex, NULL) != 0)
			return (1);
		i++;
	}
	return (0);
}

int	init_sim(t_sim *sim, t_config *config)
{
	sim->config = *config;
	sim->coders = NULL;
    sim->dongles = NULL;
	sim->stop = 0;
	sim->start_time = get_time_ms();
	if (sim->start_time < 0)
		return (1);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->state_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		return (1);
	}
	if (init_coders(sim) != 0)
		return (1);
    if (init_dongles(sim) != 0)
	    return (1);
	return (0);
}