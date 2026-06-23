#include "codexion.h"

void	destroy_sim(t_sim *sim)
{
	int	i;

    heap_destroy(&sim->scheduler);
    destroy_dongles(sim);
	if (sim->coders)
	{
		i = 0;
		while (i < sim->config.number_of_coders)
		{
			pthread_mutex_destroy(&sim->coders[i].mutex);
			i++;
		}
		free(sim->coders);
	}
	pthread_mutex_destroy(&sim->state_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
}