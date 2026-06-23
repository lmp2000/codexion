#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <limits.h>

typedef enum e_scheduler
{
	CODEXION_SCHED_FIFO,
	CODEXION_SCHED_EDF
}	t_scheduler;

typedef struct s_config
{
	int			number_of_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	int			number_of_compiles_required;
	long		dongle_cooldown;
	t_scheduler	scheduler;
}	t_config;

typedef struct s_sim	t_sim;

typedef struct s_coder // the threads
{
	int				id;
	pthread_t		thread;
	long			last_compile_time;
	int				compile_count;
	pthread_mutex_t	mutex;
	t_sim			*sim;
}	t_coder;

typedef struct s_dongle
{
	int				id;
	int				available;
	int				owner_id;
	long			cooldown_until;
	pthread_mutex_t	mutex;
}	t_dongle;

typedef struct s_request
{
	int		coder_id;
	long	request_time;
	long	deadline;
}	t_request;

typedef struct s_heap
{
	t_request	*items;
	int			size;
	int			capacity;
	t_scheduler	type;
}	t_heap;

struct s_sim // the process
{
	t_config		config;
	long			start_time;
    int				stop;
	pthread_mutex_t	log_mutex;
    pthread_mutex_t	state_mutex;
    pthread_t		monitor_thread;
    t_coder		    *coders;
    t_dongle		*dongles;
    t_heap			scheduler;
};

int	    parse_args(int argc, char **argv, t_config *config);
long	get_time_ms(void);
int		init_sim(t_sim *sim, t_config *config);
void	destroy_sim(t_sim *sim);
void	log_state(t_sim *sim, int coder_id, char *message);
int		start_simulation(t_sim *sim);
void	*coder_routine(void *arg);
int		sim_should_stop(t_sim *sim);
void	set_sim_stop(t_sim *sim);
void	precise_sleep(long duration_ms, t_sim *sim);
void	update_coder_compile_state(t_coder *coder);
long	get_coder_last_compile_time(t_coder *coder);
int		get_coder_compile_count(t_coder *coder);
void	*monitor_routine(void *arg);
int		init_dongles(t_sim *sim);
void	destroy_dongles(t_sim *sim);
int		acquire_dongles(t_coder *coder, int *first, int *second);
void	release_dongles(t_coder *coder, int first, int second);
int		heap_init(t_heap *heap, int capacity, t_scheduler type);
void	heap_destroy(t_heap *heap);
int		heap_push(t_heap *heap, t_request request);
int		heap_pop(t_heap *heap, t_request *out);
int		heap_peek(t_heap *heap, t_request *out);
int		heap_is_empty(t_heap *heap);
int		scheduler_submit_request(t_coder *coder);
int		scheduler_wait_turn(t_coder *coder);
void	scheduler_complete_request(t_coder *coder);

#endif