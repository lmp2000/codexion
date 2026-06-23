#include "codexion.h"

static void	destroy_initialized_dongles(t_sim *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

int	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->config.number_of_coders);
	if (!sim->dongles)
		return (1);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->dongles[i].id = i + 1;
		sim->dongles[i].available = 1;
		sim->dongles[i].owner_id = 0;
		sim->dongles[i].cooldown_until = sim->start_time;
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
		{
			destroy_initialized_dongles(sim, i);
			return (1);
		}
		i++;
	}
	return (0);
}

void	destroy_dongles(t_sim *sim)
{
	int	i;

	if (!sim->dongles)
		return ;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

static int	try_take_dongle(t_coder *coder, int index)
{
	t_dongle	*dongle;
	long		now;

	dongle = &coder->sim->dongles[index];
	now = get_time_ms();
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->available && now >= dongle->cooldown_until)
	{
		dongle->available = 0;
		dongle->owner_id = coder->id;
		pthread_mutex_unlock(&dongle->mutex);
		return (1);
	}
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

static void	unlock_taken_dongle(t_coder *coder, int index)
{
	t_dongle	*dongle;
	long		now;

	dongle = &coder->sim->dongles[index];
	now = get_time_ms();
	pthread_mutex_lock(&dongle->mutex);
	dongle->available = 1;
	dongle->owner_id = 0;
	dongle->cooldown_until = now + coder->sim->config.dongle_cooldown;
	pthread_mutex_unlock(&dongle->mutex);
}

int	acquire_dongles(t_coder *coder, int *first, int *second)
{
	int	i;

	*first = -1;
	*second = -1;
	while (!sim_should_stop(coder->sim))
	{
		i = 0;
		while (i < coder->sim->config.number_of_coders)
		{
			if (*first == -1 && try_take_dongle(coder, i))
				*first = i;
			else if (i != *first && try_take_dongle(coder, i))
			{
				*second = i;
                log_state(coder->sim, coder->id, "has taken a dongle");
				log_state(coder->sim, coder->id, "has taken a dongle");
				return (0);
			}
			i++;
		}
		if (*first != -1)
		{
			unlock_taken_dongle(coder, *first);
			*first = -1;
		}
		usleep(500);
	}
	return (1);
}

void	release_dongles(t_coder *coder, int first, int second)
{
	if (first >= 0)
		unlock_taken_dongle(coder, first);
	if (second >= 0)
		unlock_taken_dongle(coder, second);
}