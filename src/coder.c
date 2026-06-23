#include "codexion.h"

static void	coder_compile(t_coder *coder)
{
	int	first;
	int	second;

	if (scheduler_submit_request(coder) != 0)
		return ;
	if (scheduler_wait_turn(coder) != 0)
		return ;
	if (acquire_dongles(coder, &first, &second) != 0)
	{
		scheduler_complete_request(coder);
		return ;
	}
	update_coder_compile_state(coder);
	log_state(coder->sim, coder->id, "is compiling");
	precise_sleep(coder->sim->config.time_to_compile, coder->sim);
	release_dongles(coder, first, second);
	scheduler_complete_request(coder);
}

static void	coder_debug(t_coder *coder)
{
	log_state(coder->sim, coder->id, "is debugging");
	precise_sleep(coder->sim->config.time_to_debug, coder->sim);
}

static void	coder_refactor(t_coder *coder)
{
	log_state(coder->sim, coder->id, "is refactoring");
	precise_sleep(coder->sim->config.time_to_refactor, coder->sim);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!sim_should_stop(coder->sim)
		&& get_coder_compile_count(coder)
		< coder->sim->config.number_of_compiles_required)
	{
		coder_compile(coder);
		if (sim_should_stop(coder->sim))
			break ;
		coder_debug(coder);
		if (sim_should_stop(coder->sim))
			break ;
		coder_refactor(coder);
	}
	return (NULL);
}
