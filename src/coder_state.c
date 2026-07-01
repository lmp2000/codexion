/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_state.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	update_coder_compile_state(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile_time = get_time_ms();
	pthread_mutex_unlock(&coder->mutex);
}

void	complete_coder_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
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
