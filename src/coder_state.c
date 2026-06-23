#include "codexion.h"

void	update_coder_compile_state(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile_time = get_time_ms();
	coder->compile_count++;
	pthread_mutex_unlock(&coder->mutex);
}

long	get_coder_last_compile_time(t_coder *coder)
{
	long	last_compile_time;

	pthread_mutex_lock(&coder->mutex);
	last_compile_time = coder->last_compile_time;
	pthread_mutex_unlock(&coder->mutex);
	return (last_compile_time);
}

int	get_coder_compile_count(t_coder *coder)
{
	int	compile_count;

	pthread_mutex_lock(&coder->mutex);
	compile_count = coder->compile_count;
	pthread_mutex_unlock(&coder->mutex);
	return (compile_count);
}