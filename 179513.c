uint64_to_str_back(char *ptr, guint64 value)
{
	char const *p;

	/* special case */
	if (value == 0)
		*(--ptr) = '0';

	while (value >= 10) {
		p = fast_strings[100 + (value % 100)];

		value /= 100;

		*(--ptr) = p[2];
		*(--ptr) = p[1];
	}

	/* value will be 0..9, so using '& 0xF' is safe, and faster than '% 10' */
	if (value)
		*(--ptr) = (value & 0xF) | '0';

	return ptr;
}