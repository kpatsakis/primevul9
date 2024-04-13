static double timelib_get_frac_nr(char **ptr, int max_length)
{
	char *begin, *end, *str;
	double tmp_nr = TIMELIB_UNSET;
	int len = 0;

	while ((**ptr != '.') && (**ptr != ':') && ((**ptr < '0') || (**ptr > '9'))) {
		if (**ptr == '\0') {
			return TIMELIB_UNSET;
		}
		++*ptr;
	}
	begin = *ptr;
	while (((**ptr == '.') || (**ptr == ':') || ((**ptr >= '0') && (**ptr <= '9'))) && len < max_length) {
		++*ptr;
		++len;
	}
	end = *ptr;
	str = timelib_calloc(1, end - begin + 1);
	memcpy(str, begin, end - begin);
	if (str[0] == ':') {
		str[0] = '.';
	}
	tmp_nr = strtod(str, NULL);
	timelib_free(str);
	return tmp_nr;
}