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
            if (sim->coders[i].mutex_ready)
                pthread_mutex_destroy(&sim->coders[i].mutex);
            i++;
        }
        free(sim->coders);
        sim->coders = NULL;
    }
	if (sim->scheduler_cond_ready)
	{
		pthread_cond_destroy(&sim->scheduler_cond);
		sim->scheduler_cond_ready = 0;
	}
	if (sim->scheduler_mutex_ready)
	{
		pthread_mutex_destroy(&sim->scheduler_mutex);
		sim->scheduler_mutex_ready = 0;
	}
	if (sim->state_mutex_ready)
	{
		pthread_mutex_destroy(&sim->state_mutex);
		sim->state_mutex_ready = 0;
	}
	if (sim->log_mutex_ready)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		sim->log_mutex_ready = 0;
	}
}