/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lude-jes <lude-jes@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by lude-jes          #+#    #+#             */
/*   Updated: 2026/07/01 15:47:10 by lude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	validate_config(t_config *config)
{
	if (config->number_of_coders <= 0)
		return (1);
	if (config->number_of_compiles_required <= 0)
		return (1);
	return (0);
}

static int	parse_scheduler(const char *str, t_scheduler *scheduler)
{
	if (codexion_strcmp(str, "fifo") == 0)
	{
		*scheduler = CODEXION_SCHED_FIFO;
		return (0);
	}
	if (codexion_strcmp(str, "edf") == 0)
	{
		*scheduler = CODEXION_SCHED_EDF;
		return (0);
	}
	return (1);
}

int	parse_args(int argc, char **argv, t_config *config)
{
	if (argc != 9)
		return (1);
	if (parse_positive_int(argv[1], &config->number_of_coders) != 0)
		return (1);
	if (parse_positive_long(argv[2], &config->time_to_burnout) != 0)
		return (1);
	if (parse_positive_long(argv[3], &config->time_to_compile) != 0)
		return (1);
	if (parse_positive_long(argv[4], &config->time_to_debug) != 0)
		return (1);
	if (parse_positive_long(argv[5], &config->time_to_refactor) != 0)
		return (1);
	if (parse_positive_int(argv[6], &config->number_of_compiles_required) != 0)
		return (1);
	if (parse_positive_long(argv[7], &config->dongle_cooldown) != 0)
		return (1);
	if (parse_scheduler(argv[8], &config->scheduler) != 0)
		return (1);
	if (validate_config(config) != 0)
		return (1);
	return (0);
}
