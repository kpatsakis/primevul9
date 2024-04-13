static timelib_sll timelib_meridian(char **ptr, timelib_sll h)
{
	timelib_sll retval = 0;

	while (!strchr("AaPp", **ptr)) {
		++*ptr;
	}
	if (**ptr == 'a' || **ptr == 'A') {
		if (h == 12) {
			retval = -12;
		}
	} else if (h != 12) {
		retval = 12;
	}
	++*ptr;
	if (**ptr == '.') {
		++*ptr;
	}
	if (**ptr == 'M' || **ptr == 'm') {
		++*ptr;
	}
	if (**ptr == '.') {
		++*ptr;
	}
	return retval;
}