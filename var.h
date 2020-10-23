#pragma once

#include <stdlib.h>

#include "string.h"

#define VAR struct var_node_t
#define TYPE enum var_type_t

#define VAR_VAL_INT(a) (*(int*)a)
#define VAR_VAL_FLOAT(a) (*(float*)a)
#define VAR_VAL_STRING(a) ((STRING*)a)

enum var_type_t
{
	INT_T = 0,
	FLOAT_T = 1,
	STRING_T = 2,
	UNKNOWN_T = 3
};

struct var_node_t
{
	STRING *name;
	void *value;
	TYPE type;

	VAR *next;
};

VAR *VAR_GLOBAL_HEAD;
VAR *VAR_TEMP;

VAR *var_add(VAR *var)
{
	VAR *temp;

	if (!VAR_GLOBAL_HEAD)
	{
		VAR_GLOBAL_HEAD = var;
		return;
	}

	temp = VAR_GLOBAL_HEAD;
	while (temp->next)
	{
		temp = temp->next;
	}
	temp->next = var;

	return var;
}

VAR *var_find(STRING *name)
{
	VAR *out;
	out = VAR_GLOBAL_HEAD;
	
	while (out)
	{
		if (str_compare(out->name, name)) return out;

		out = out->next;
	}

	return NULL;
}

VAR *var_create(STRING *name, void *value, TYPE type)
{
	VAR *out;

	out = (VAR *)malloc(sizeof(VAR));
	if (!out) return out;

	out->name = name;
	out->value = value;
	out->type = type;
	out->next = NULL;

	return out;
}

VAR *var_create_and_add(STRING *name, void *value, TYPE type)
{
	return var_add(var_create(name, value, type));
}

VAR *var_create_empty()
{
	VAR *out;

	out = (VAR*)malloc(sizeof(VAR));
	if (!out) return out;

	out->name = NULL;
	out->value = NULL;
	out->type = INT_T;
	out->next = NULL;

	return out;
}

void *var_create_type(TYPE type)
{
	void *out;

	switch (type)
	{
	case INT_T:
		out = malloc(sizeof(int));
		if (!out) return NULL;

		VAR_VAL_INT(out) = 0;
		break;
	case FLOAT_T:
		out = malloc(sizeof(float));
		if (!out) return NULL;

		VAR_VAL_FLOAT(out) = 0;
		break;
	case STRING_T:
		out = malloc(sizeof(STRING*));
		if (!out) return NULL;

		*VAR_VAL_STRING(out) = TERM;
		break;
	default:
		return NULL;
		break;
	}

	return out;
}

void *var_create_int(int val)
{
	void *out;

	out = var_create_type(INT_T);
	VAR_VAL_INT(out) = val;

	return out;
}
void *var_create_float(float val)
{
	void *out;

	out = var_create_type(FLOAT_T);
	VAR_VAL_FLOAT(out) = val;

	return out;
}
void *var_create_string(STRING *val)
{
	void *out;

	out = var_create_type(STRING_T);
	VAR_VAL_STRING(out) = val;

	return out;
}

void var_init()
{
	VAR_TEMP = var_create("__TEMP", NULL, INT_T);
}

STRING *var_type_tostring(TYPE type)
{
	STRING *out;

	switch (type)
	{
	case INT_T:
		return str_create_copy("INT");
		break;
	case FLOAT_T:
		return str_create_copy("FLOAT");
		break;
	case STRING_T:
		return str_create_copy("STRING");
		break;
	default:
		return str_create_copy("UNKNOWN");
		break;
	}
}

TYPE var_string_to_type(STRING *str)
{
	if (str_compare_const(str, "int")) return INT_T;
	else if (str_compare_const(str, "float")) return FLOAT_T;
	else if (str_compare_const(str, "string")) return STRING_T;
	else return UNKNOWN_T;
}

TYPE var_infer_type(STRING *val)
{
	if (str_contains_nonnumeric(val) > -1) return STRING_T;
	else if (str_contains(val, '.') > -1) return FLOAT_T;
	else return INT_T;
}

void *var_string_to_value(STRING *str, TYPE type)
{
	STRING *out;
	switch (type)
	{
	case INT_T:
		return var_create_int(atoi(str));
		break;
	case FLOAT_T:
		return var_create_float(atof(str));
		break;
	case STRING_T:
		if (str[0] == '\'')
		{
			str++;
			out = str_create_copy(str);
			out[str_length(out) - 1] = TERM;
			return out;
		}
		return str_create_copy(str);
		break;
	default:
		return NULL;
		break;
	}
}

VAR *var_free(VAR *var)
{
	VAR *out;

	if (!var) return NULL;

	if (var->name)
	{
		free(var->name);
	}
	if (var->value)
	{
		free(var->value);
	}
	out = var->next;
	free(var);

	return out;
}

void var_free_all()
{
	VAR *node;
	node = VAR_GLOBAL_HEAD;

	while (node)
	{
		node = var_free(node);
	}
}

void var_delete(STRING *name)
{
	VAR *var;

	if (!VAR_GLOBAL_HEAD) return;

	if (str_compare(VAR_GLOBAL_HEAD->name, name))
	{
		var = VAR_GLOBAL_HEAD->next;
		var_free(VAR_GLOBAL_HEAD);
		VAR_GLOBAL_HEAD = var;
		return;
	}

	var = VAR_GLOBAL_HEAD;
	while (var->next)
	{
		if (str_compare(var->next->name, name))
		{
			var->next = var_free(var->next);
			return;
		}
		var = var->next;
	}
}

void var_dump_all()
{
	VAR *node;
	STRING *type;
	unsigned int i;

	i = 0;
	node = VAR_GLOBAL_HEAD;

	while (node)
	{
		type = var_type_tostring(node->type);
		printf("[VAR %u]\n\tNAME = %s\n\tTYPE = %s\n\t", i, node->name, type);
		free(type);

		if (node->type == INT_T) printf("VAL  = %i\n\t", VAR_VAL_INT(node->value));
		else if (node->type == FLOAT_T) printf("VAL  = %f\n\t", VAR_VAL_FLOAT(node->value));
		else if (node->type == STRING_T) printf("VAL  = %s\n\t", VAR_VAL_STRING(node->value));
		else printf("VAL  = ???\n\t");

		printf("ADDR = %p\n", node);

		i++;
		node = node->next;
	}
}

#pragma region var value operations

void var_copy_value(VAR *a, VAR *b)
{
	if (a == NULL || b == NULL) return;

	a->type = b->type;

	if(a->value) free(a->value);

	if (b->type == INT_T) a->value = var_create_int(VAR_VAL_INT(b->value));
	else if (b->type == FLOAT_T) a->value = var_create_float(VAR_VAL_FLOAT(b->value));
	else if (b->type == STRING_T) a->value = str_create_copy(VAR_VAL_STRING(b->value));
}

void var_value_set(VAR *var, void *value, TYPE type)
{
	void *temp;
	switch (var->type)
	{

	case INT_T:

		if (type == INT_T)
		{
			VAR_VAL_INT(var->value) = VAR_VAL_INT(value);
		}
		else if (type == FLOAT_T)
		{
			var->type = FLOAT_T;

			temp = var_create_type(FLOAT_T);
			VAR_VAL_FLOAT(temp) = VAR_VAL_FLOAT(value);

			free(var->value);
			var->value = temp;
		}
		else if (type == STRING_T)
		{
			var->type = STRING_T;

			temp = str_create_copy(VAR_VAL_STRING(value));

			free(var->value);
			var->value = temp;
		}

		break;

	case FLOAT_T:

		if (type == INT_T)
		{
			VAR_VAL_FLOAT(var->value) = VAR_VAL_INT(value);
		}
		else if (type == FLOAT_T)
		{
			VAR_VAL_FLOAT(var->value) = VAR_VAL_FLOAT(value);
		}
		else if (type == STRING_T)
		{
			var->type = STRING_T;

			temp = str_create_copy(VAR_VAL_STRING(value));

			free(var->value);
			var->value = temp;
		}

		break;

	case STRING_T:

		if (type == INT_T)
		{
			temp = str_int_tostring(VAR_VAL_INT(value));

			free(var->value);
			var->value = temp;
		}
		else if (type == FLOAT_T)
		{
			temp = str_float_tostring(VAR_VAL_FLOAT(value));

			free(var->value);
			var->value = temp;
		}
		else if (type == STRING_T)
		{
			temp = str_create_copy(VAR_VAL_STRING(value));

			free(var->value);
			var->value = temp;
		}

		break;

	default:
		break;
	}
}
void var_vars_set(VAR *a, VAR *b)
{
	var_value_set(a, b->value, b->type);
}

void var_value_add(VAR *var, void *value, TYPE type)
{
	void *temp;
	switch (var->type)
	{

	case INT_T:

		if (type == INT_T)
		{
			VAR_VAL_INT(var->value) += VAR_VAL_INT(value);
		}
		else if (type == FLOAT_T)
		{
			var->type = FLOAT_T;

			temp = var_create_type(FLOAT_T);
			VAR_VAL_FLOAT(temp) = (float)VAR_VAL_INT(var->value) + VAR_VAL_FLOAT(value);

			free(var->value);
			var->value = temp;
		}
		else if (type == STRING_T)
		{
			var->type = STRING_T;

			temp = str_int_tostring(VAR_VAL_INT(var->value));
			temp = str_append(VAR_VAL_STRING(temp), VAR_VAL_STRING(value));

			free(var->value);
			var->value = temp;
		}

		break;

	case FLOAT_T:

		if (type == INT_T)
		{
			VAR_VAL_FLOAT(var->value) += VAR_VAL_INT(value);
		}
		else if (type == FLOAT_T)
		{
			VAR_VAL_FLOAT(var->value) += VAR_VAL_FLOAT(value);
		}
		else if (type == STRING_T)
		{
			var->type = STRING_T;

			temp = str_float_tostring(VAR_VAL_FLOAT(var->value));
			temp = str_append(VAR_VAL_STRING(temp), VAR_VAL_STRING(value));

			free(var->value);
			var->value = temp;
		}

		break;

	case STRING_T:

		if (type == INT_T)
		{
			temp = str_int_tostring(VAR_VAL_INT(value));
			VAR_VAL_STRING(var->value) = str_append(VAR_VAL_STRING(var->value), temp);
			free(temp);
		}
		else if (type == FLOAT_T)
		{
			temp = str_float_tostring(VAR_VAL_FLOAT(value));
			VAR_VAL_STRING(var->value) = str_append(VAR_VAL_STRING(var->value), temp);
			free(temp);
		}
		else if (type == STRING_T)
		{
			VAR_VAL_STRING(var->value) = str_append(VAR_VAL_STRING(var->value), VAR_VAL_STRING(value));
		}

		break;

	default:
		break;
	}
}
void var_vars_add(VAR *a, VAR *b)
{
	var_value_add(a, b->value, b->type);
}

void var_value_subtract(VAR *var, void *value, TYPE type)
{
	void *temp;
	switch (var->type)
	{

	case INT_T:

		if (type == INT_T)
		{
			VAR_VAL_INT(var->value) -= VAR_VAL_INT(value);
		}
		else if (type == FLOAT_T)
		{
			var->type = FLOAT_T;

			temp = var_create_type(FLOAT_T);
			VAR_VAL_FLOAT(temp) = (float)VAR_VAL_INT(var->value) - VAR_VAL_FLOAT(value);

			free(var->value);
			var->value = temp;
		}
		else if (type == STRING_T)
		{
			var->type = STRING_T;

			temp = str_int_tostring(VAR_VAL_INT(var->value));
			temp = str_remove(VAR_VAL_STRING(temp), VAR_VAL_STRING(value));

			free(var->value);
			var->value = temp;
		}

		break;

	case FLOAT_T:

		if (type == INT_T)
		{
			VAR_VAL_FLOAT(var->value) -= VAR_VAL_INT(value);
		}
		else if (type == FLOAT_T)
		{
			VAR_VAL_FLOAT(var->value) -= VAR_VAL_FLOAT(value);
		}
		else if (type == STRING_T)
		{
			var->type = STRING_T;

			temp = str_float_tostring(VAR_VAL_FLOAT(var->value));
			temp = str_remove(VAR_VAL_STRING(temp), VAR_VAL_STRING(value));

			free(var->value);
			var->value = temp;
		}

		break;

	case STRING_T:

		if (type == INT_T)
		{
			temp = str_int_tostring(VAR_VAL_INT(value));
			VAR_VAL_STRING(var->value) = str_remove(VAR_VAL_STRING(var->value), temp);
			free(temp);
		}
		else if (type == FLOAT_T)
		{
			temp = str_float_tostring(VAR_VAL_FLOAT(value));
			VAR_VAL_STRING(var->value) = str_remove(VAR_VAL_STRING(var->value), temp);
			free(temp);
		}
		else if (type == STRING_T)
		{
			VAR_VAL_STRING(var->value) = str_remove(VAR_VAL_STRING(var->value), VAR_VAL_STRING(value));
		}

		break;

	default:
		break;
	}
}
void var_vars_subtract(VAR *a, VAR *b)
{
	var_value_subtract(a, b->value, b->type);
}

void var_value_mult(VAR *var, void *value, TYPE type)
{
	void *temp;
	switch (var->type)
	{

	case INT_T:

		if (type == INT_T)
		{
			VAR_VAL_INT(var->value) *= VAR_VAL_INT(value);
		}
		else if (type == FLOAT_T)
		{
			var->type = FLOAT_T;

			temp = var_create_type(FLOAT_T);
			VAR_VAL_FLOAT(temp) = (float)VAR_VAL_INT(var->value) * VAR_VAL_FLOAT(value);

			free(var->value);
			var->value = temp;
		}
		else if (type == STRING_T)
		{
			printf("ERROR: Multiplication operator cannot be used between int and string.\n");
		}

		break;

	case FLOAT_T:

		if (type == INT_T)
		{
			VAR_VAL_FLOAT(var->value) *= VAR_VAL_INT(value);
		}
		else if (type == FLOAT_T)
		{
			VAR_VAL_FLOAT(var->value) *= VAR_VAL_FLOAT(value);
		}
		else if (type == STRING_T)
		{
			printf("ERROR: Multiplication operator cannot be used between float and string.\n");
		}

		break;

	case STRING_T:

		printf("ERROR: Multiplication operator cannot be used with a string.\n");

		break;

	default:
		break;
	}
}
void var_vars_mult(VAR *a, VAR *b)
{
	var_value_mult(a, b->value, b->type);
}

void var_value_div(VAR *var, void *value, TYPE type)
{
	void *temp;
	switch (var->type)
	{

	case INT_T:

		if (type == INT_T)
		{
			VAR_VAL_INT(var->value) /= VAR_VAL_INT(value);
		}
		else if (type == FLOAT_T)
		{
			var->type = FLOAT_T;

			temp = var_create_type(FLOAT_T);
			VAR_VAL_FLOAT(temp) = (float)VAR_VAL_INT(var->value) / VAR_VAL_FLOAT(value);

			free(var->value);
			var->value = temp;
		}
		else if (type == STRING_T)
		{
			printf("ERROR: Division operator cannot be used between int and string.\n");
		}

		break;

	case FLOAT_T:

		if (type == INT_T)
		{
			VAR_VAL_FLOAT(var->value) /= VAR_VAL_INT(value);
		}
		else if (type == FLOAT_T)
		{
			VAR_VAL_FLOAT(var->value) /= VAR_VAL_FLOAT(value);
		}
		else if (type == STRING_T)
		{
			printf("ERROR: Division operator cannot be used between float and string.\n");
		}

		break;

	case STRING_T:

		printf("ERROR: Division operator cannot be used with a string.\n");

		break;

	default:
		break;
	}
}
void var_vars_div(VAR *a, VAR *b)
{
	var_value_div(a, b->value, b->type);
}

#pragma endregion