#include <stdlib.h>
#include "config_handler.h"

int main(void)
{
	struct boot_option **boot_options = NULL;
	int size = 0;
	int line_number = 0;
	parse_config_file(&boot_options, &size, &line_number);


	print_file(CONFIG_FILE, 0, line_number -1);

	for (int i = 0; i < size; i++) {
		print_boot_option(boot_options[i]);
		printf("\n");
	}

	exit(EXIT_SUCCESS);
}
