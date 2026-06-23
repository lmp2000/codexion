#include "codexion.h"

void	log_state(t_sim *sim, int coder_id, char *message)
{
	long	timestamp;

	timestamp = get_time_ms() - sim->start_time;
	pthread_mutex_lock(&sim->log_mutex);
	printf("%ld %d %s\n", timestamp, coder_id, message);
	pthread_mutex_unlock(&sim->log_mutex);
}