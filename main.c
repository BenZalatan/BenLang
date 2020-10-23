#include <stdio.h>

#include "var.h"
#include "string.h"
#include "parser.h"

int main()
{
	char input[100];

	var_init();

	while (1)
	{
		printf("> ");
		gets(input);
		if (str_compare_const(input, "quit")) break;

		LAST_CMD = str_create_copy(input);
		parser_run_command();
	}

	printf("Quitting...\n");
	var_free_all();

	return 0;
}