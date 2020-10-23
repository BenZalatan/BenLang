#pragma once

#include "var.h"
#include "string.h"

STRING *CODE, *LAST_CMD, *CMD_POS;
int CMD_MOVE;
char CMD_HAS_BEGUN;

enum PARSER_OP
{
	OP_NONE = 0,
	OP_ADD = 1,
	OP_SUB = 2,
	OP_MULT = 3,
	OP_DIV = 4
};

enum PARSER_OP parser_op_from_string(STRING *str)
{
	switch (str[0])
	{
	case '+':
		return OP_ADD;
	case '-':
		return OP_SUB;
	case '*':
		return OP_MULT;
	case '/':
		return OP_DIV;
	default:
		return OP_NONE;
	}
}

int parser_zero_next_whitespace()
{
	int pos;
	char quotations;

	if (!CMD_POS) return -1;

	pos = 0;
	quotations = 0;

	while (CMD_POS[pos])
	{
		if (CMD_POS[pos] == '\'') quotations = !quotations;

		if (!quotations)
		{
			if (CMD_POS[pos] == ' ' ||
				CMD_POS[pos] == '\t' ||
				CMD_POS[pos] == '\n') break;
		}

		pos++;
	}

	if (CMD_POS[pos] == TERM) return -1;

	CMD_POS[pos] = TERM;

	return pos;
}

void parser_skip_whitespace()
{
	while (CMD_POS && CMD_POS[0] &&
		(
			CMD_POS[0] == ' ' ||
			CMD_POS[0] == '\t' ||
			CMD_POS[0] == '\n'
			)) CMD_POS++;
	if (CMD_POS[0] == TERM) CMD_POS = NULL;
}

char parser_next_word()
{
	if (CMD_MOVE < 0)
	{
		return 0;
	}
	CMD_POS += CMD_MOVE + (CMD_HAS_BEGUN ? 1 : 0);
	CMD_HAS_BEGUN = 1;
	parser_skip_whitespace();
	CMD_MOVE = parser_zero_next_whitespace();
	return 1;
}

void parser_eval_exp(struct var_node_t *out)
{
	struct var_node_t *base, *temp;
	enum PARSER_OP last_op, curr_op;
	char first_term;

	/*CMD_POS = LAST_CMD;
	CMD_MOVE = 0;
	CMD_HAS_BEGUN = 0;*/

	temp = var_create_empty();
	last_op = curr_op = OP_NONE;
	first_term = 1;

	base = var_create_empty();
	base->type = INT_T;
	base->value = var_create_int(0);

	/* assuming the expression is at CMD_POS */
	while (parser_next_word())
	{
		if (last_op == OP_NONE && !first_term)
		{
			curr_op = parser_op_from_string(CMD_POS);
			if (curr_op != OP_NONE)
			{
				last_op = curr_op;
				continue;
			}
			else
			{
				printf("ERROR: Unknown operator '%s'.\n", CMD_POS);
				free(temp);
				return;
			}
		}

		if (CMD_POS[0] == '@')
		{
			var_copy_value(temp, var_find(CMD_POS + 1));
		}
		else
		{
			temp->type = var_infer_type(CMD_POS);
			temp->value = var_string_to_value(CMD_POS, temp->type);
		}

		if (first_term)
		{
			var_copy_value(base, temp);
			first_term = 0;
			continue;
		}

		switch (last_op)
		{
		case OP_ADD:
			var_vars_add(base, temp);
			break;
		case OP_SUB:
			var_vars_subtract(base, temp);
			break;
		case OP_MULT:
			var_vars_mult(base, temp);
			break;
		case OP_DIV:
			var_vars_div(base, temp);
			break;
		}

		last_op = OP_NONE;

		free(temp->value);
	}

	out->type = base->type;
	out->value = base->value;

	free(temp);
	free(base);
}

void parser_run_command()
{
	/* assume LAST_CMD is set */
	VAR *var;
	TYPE type;
	STRING *str;
	void *val;

	CMD_POS = LAST_CMD;
	CMD_MOVE = 0;
	CMD_HAS_BEGUN = 0;
	parser_next_word();

	if (str_compare_const(CMD_POS, "var"))
	{
		/* name arg */
		parser_next_word();

		var = var_create_empty();
		var->name = str_create_copy(CMD_POS);

		/* operator arg */

		if (!parser_next_word())
		{
			/* assume int */
			var->type = INT_T;
			var->value = var_create_int(0);
		}
		else if (str_compare_const(CMD_POS, ":"))
		{
			/* set type */

			parser_next_word();

			var->type = var_string_to_type(CMD_POS);
			var->value = var_create_type(var->type);
		}
		else if (str_compare_const(CMD_POS, "="))
		{
			/* set value */

			parser_eval_exp(var);
		}
		else
		{
			printf("ERROR: Unknown operator '%s'.\n", CMD_POS);

			var_free(var);

			goto exit;
		}

		var_add(var);
	}
	else if (str_compare_const(CMD_POS, "eval"))
	{
		var = var_create_empty();
		parser_eval_exp(var);
		str = var_type_tostring(var->type);
		printf("%s: ", str);
		if (var->type == INT_T) printf("%i\n", VAR_VAL_INT(var->value));
		else if (var->type == FLOAT_T) printf("%f\n", VAR_VAL_FLOAT(var->value));
		else if (var->type == STRING_T) printf("%s\n", VAR_VAL_STRING(var->value));
		else printf("???\n\t");
	}
	else if (str_compare_const(CMD_POS, "input"))
	{
		if (!parser_next_word())
		{
			printf("ERROR: Variable name required for 'input'.\n");
			goto exit;
		}
		var = var_find(CMD_POS);

		if (!var)
		{
			printf("ERROR: Unknown variable '%s'.\n", CMD_POS);
		}

		str = (STRING *)malloc(100); /* 100 char long input */
		printf("Input: ");
		gets(str);

		var->type = var_infer_type(str);

		if(var->value) free(var->value);
		var->value = var_string_to_value(str, var->type);

		free(str);
	}
	else if (str_compare_const(CMD_POS, "del"))
	{
		parser_next_word();
		var_delete(CMD_POS);
	}
	else if (str_compare_const(CMD_POS, "dump"))
	{
		var_dump_all();
	}
	else if (str_compare_const(CMD_POS, "clear"))
	{
		system("cls");
	}
	else
	{
		var = var_find(CMD_POS);
		if (!var)
		{
			printf("ERROR: Unknown command '%s'.\n", CMD_POS);

			goto exit;
		}

		parser_next_word();
		if (str_compare_const(CMD_POS, ":"))
		{
			/* set type */

			parser_next_word();
			
			type = var_string_to_type(CMD_POS);

			if (type == UNKNOWN_T)
			{
				printf("ERROR: Unknown type '%s'.\n", CMD_POS);
				
				goto exit;
			}

			if (var->type == type) goto exit;

			switch (var->type)
			{
			case INT_T:

				if (type == FLOAT_T)
				{
					val = var->value;
					var->type = FLOAT_T;
					var->value = var_create_float((float)VAR_VAL_INT(var->value));
					free(val);
				}
				else if (type == STRING_T)
				{
					val = var->value;
					var->type = STRING_T;
					var->value = str_int_tostring(VAR_VAL_INT(var->value));
					free(val);
				}

				break;
			case FLOAT_T:

				if (type == INT_T)
				{
					val = var->value;
					var->type = INT_T;
					var->value = var_create_int((int)VAR_VAL_INT(var->value));
					free(val);
				}
				else if (type == STRING_T)
				{
					val = var->value;
					var->type = STRING_T;
					var->value = str_float_tostring(VAR_VAL_FLOAT(var->value));
					free(val);
				}

				break;
			case STRING_T:

				if (type == INT_T)
				{
					val = var->value;
					var->type = INT_T;
					var->value = var_create_int(atoi(var->value));
					free(val);
				}
				else if (type == FLOAT_T)
				{
					val = var->value;
					var->type = STRING_T;
					var->value = var_create_float(atof(var->value));
					free(val);
				}

				break;
			default:
				break;
			}
		}
		else if (str_compare_const(CMD_POS, "="))
		{
			parser_eval_exp(var);
		}
		else
		{
			printf("ERROR: Unknown operator '%s'.\n", CMD_POS);

			goto exit;
		}
	}

exit:

	free(LAST_CMD);
}