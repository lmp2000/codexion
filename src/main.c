#include "codexion.h"

int	main(int argc, char **argv)
{
	t_config	config;
	t_sim		sim;

	if (parse_args(argc, argv, &config) != 0)
	{
		printf("Error: invalid arguments\n");
		return (1);
	}
	if (init_sim(&sim, &config) != 0)
	{
		printf("Error: simulation init failed\n");
		return (1);
	}
	if (start_simulation(&sim) != 0)
	{
		printf("Error: simulation failed\n");
		destroy_sim(&sim);
		return (1);
	}
	destroy_sim(&sim);
	return (0);
}