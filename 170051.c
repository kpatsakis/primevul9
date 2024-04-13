static timelib_long timelib_lookup_abbr(char **ptr, int *dst, char **tz_abbr, int *found)
{
	char *word;
	char *begin = *ptr, *end;
	timelib_long  value = 0;
	const timelib_tz_lookup_table *tp;

	while (**ptr != '\0' && **ptr != ')' && **ptr != ' ') {
		++*ptr;
	}
	end = *ptr;
	word = timelib_calloc(1, end - begin + 1);
	memcpy(word, begin, end - begin);

	if ((tp = abbr_search(word, -1, 0))) {
		value = -tp->gmtoffset / 60;
		*dst = tp->type;
		value += tp->type * 60;
		*found = 1;
	} else {
		*found = 0;
	}

	*tz_abbr = word;
	return value;
}