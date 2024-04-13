static timelib_long timelib_lookup_month(char **ptr)
{
	char *word;
	char *begin = *ptr, *end;
	timelib_long  value = 0;
	const timelib_lookup_table *tp;

	while ((**ptr >= 'A' && **ptr <= 'Z') || (**ptr >= 'a' && **ptr <= 'z')) {
		++*ptr;
	}
	end = *ptr;
	word = timelib_calloc(1, end - begin + 1);
	memcpy(word, begin, end - begin);

	for (tp = timelib_month_lookup; tp->name; tp++) {
		if (strcasecmp(word, tp->name) == 0) {
			value = tp->value;
		}
	}

	timelib_free(word);
	return value;
}