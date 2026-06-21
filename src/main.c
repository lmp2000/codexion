#include "codexion.h"

int	main(int argc, char **argv)
{
	t_config	config;

	if (parse_args(argc, argv, &config) != 0)
	{
		printf("Error: invalid arguments\n");
		return (1);
	}
	printf("Codexion config loaded successfully\n");
	return (0);
}