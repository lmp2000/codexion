#include "codexion.h"

static int	validate_config(t_config *config)
{
	if (config->number_of_coders <= 0)
		return (1);
	if (config->number_of_compiles_required <= 0)
		return (1);
	return (0);
}

static int	ft_strcmp(const char *s1, const char *s2)
{
	int	i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

static int	is_digit_string(const char *str)
{
	int	i;

	if (!str || !str[0])
		return (0);
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	parse_positive_long(const char *str, long *out)
{
	long	result;
	int		i;

	if (!is_digit_string(str))
		return (1);
	result = 0;
	i = 0;
	while (str[i])
	{
		if (result > (LONG_MAX - (str[i] - '0')) / 10)
			return (1);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	*out = result;
	return (0);
}

static int	parse_positive_int(const char *str, int *out)
{
	long	value;

	if (parse_positive_long(str, &value) != 0)
		return (1);
	if (value > INT_MAX)
		return (1);
	*out = (int)value;
	return (0);
}

static int	parse_scheduler(const char *str, t_scheduler *scheduler)
{
	if (ft_strcmp(str, "fifo") == 0)
	{
		*scheduler = CODEXION_SCHED_FIFO;
		return (0);
	}
	if (ft_strcmp(str, "edf") == 0)
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