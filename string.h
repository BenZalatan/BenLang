#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#define STRING unsigned char
#define TERM '\0'

unsigned int str_length(STRING *str)
{
	unsigned int out;
	out = 0;

	while (str[out]) out++;

	return out;
}

void str_copy(STRING *dest, STRING *from)
{
	unsigned int i;
	i = 0;

	while (from[i])
	{
		dest[i] = from[i];

		i++;
	}
	dest[i] = TERM;
}

STRING *str_create_copy(STRING *str)
{
	STRING *out;

	out = (STRING *)malloc(sizeof(STRING) * (str_length(str) + 1));
	str_copy(out, str);

	return out;
}

int str_contains(STRING *str, char c)
{
	unsigned int i;
	i = 0;

	while (str[i])
	{
		if (str[i] == c) return i;
		i++;
	}
	return -1;
}

int str_contains_nonnumeric(STRING *str)
{
	unsigned int i;
	i = 0;

	while (str[i])
	{
		if ((str[i] < '0' || str[i] > '9') &&
			str[i] != '.' && str[i] != '-') return i;
		i++;
	}
	return -1;
}

int str_index_of(STRING *a, STRING *b)
{
	int out;
	unsigned int a_i, b_i, b_len;

	a_i = b_i = 0;
	b_len = str_length(b);

	while (a[a_i])
	{
		b_i = 0;
		while (b[b_i])
		{
			if (!a[a_i + b_i] || a[a_i + b_i] != b[b_i]) break;
			b_i++;
		}
		if (b_i == b_len) return a_i;

		a_i++;
	}

	return -1;
}

char str_compare(STRING *a, STRING *b)
{
	unsigned int i;
	i = 0;

	if (!a || !b) return 0;

	while (a[i] && b[i])
	{
		if (a[i] != b[i]) return 0;
		i++;
	}
	if (a[i] || b[i]) return 0;

	return 1;
}

char str_compare_const(STRING *a, const char *b)
{
	unsigned int i;
	i = 0;

	while (a[i] && b[i])
	{
		if (a[i] != b[i]) return 0;
		i++;
	}
	if (a[i] || b[i]) return 0;

	return 1;
}

STRING *str_remove(STRING *a, STRING *b)
{
	int index;
	unsigned int a_len, b_len, i;
	STRING *temp;

	index = str_index_of(a, b);
	if (index < 0) return a;

	a_len = str_length(a);
	b_len = str_length(b);

	temp = (STRING *)malloc(sizeof(STRING) * (a_len - b_len + 1));
	if (!temp) return NULL;
	
	for (i = 0; i < a_len + 1; i++)
	{
		if (i == index) i += b_len;

		if (i < index) temp[i] = a[i];
		else if (i > index) temp[i - b_len] = a[i];
	}

	free(a);

	return temp;
}

STRING *str_append(STRING *a, STRING *b)
{
	STRING *out;
	unsigned int a_len, b_len;

	a_len = str_length(a);
	b_len = str_length(b);

	out = (STRING *)malloc(sizeof(STRING) * (a_len + b_len + 1));
	if (!out) return NULL;

	str_copy(out, a);
	str_copy(out + a_len, b);
	out[a_len + b_len] = TERM;

	free(a);
	return out;
}

#define STR_INT_LEN(a) (unsigned int)((ceil(log10((double)a)) + 1) * sizeof(STRING))

STRING *str_int_tostring(int i)
{
	STRING *out;
	unsigned int len;

	len = STR_INT_LEN(i);

	out = (STRING *)malloc(len + 1);
	if (!out) return NULL;

	sprintf_s((char*)out, len + 1, "%i", i);
	out[len] = TERM;

	return out;
}

STRING *str_float_tostring(float f)
{
	STRING *out;

	out = (STRING *)malloc(25 /* max length of 24 + 1 term */);
	if (!out) return NULL;

	sprintf_s((char *)out, 24, "%f", f);
	out[24] = TERM;

	return out;
}