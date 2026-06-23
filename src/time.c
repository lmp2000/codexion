#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (-1);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	precise_sleep(long duration_ms, t_sim *sim)
{
	long	start;

	start = get_time_ms();
	while (!sim_should_stop(sim) && get_time_ms() - start < duration_ms)
		usleep(500);
}
