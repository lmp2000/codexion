#include "codexion.h"

int	sim_should_stop(t_sim *sim)
{
	int	should_stop;

	pthread_mutex_lock(&sim->state_mutex);
	should_stop = sim->stop;
	pthread_mutex_unlock(&sim->state_mutex);
	return (should_stop);
}

void	set_sim_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->state_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->state_mutex);
}