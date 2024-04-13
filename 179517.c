uint_to_str_back(char *ptr, guint32 value)
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

	if (value)
		*(--ptr) = (value) | '0';

	return ptr;
}