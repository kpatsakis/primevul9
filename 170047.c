static timelib_sll timelib_meridian_with_check(char **ptr, timelib_sll h)
{
	timelib_sll retval = 0;

	while (**ptr && !strchr("AaPp", **ptr)) {
		++*ptr;
	}
	if(!**ptr) {
		return TIMELIB_UNSET;
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
		if (**ptr != 'm' && **ptr != 'M') {
			return TIMELIB_UNSET;
		}
		++*ptr;
		if (**ptr != '.' ) {
			return TIMELIB_UNSET;
		}
		++*ptr;
	} else if (**ptr == 'm' || **ptr == 'M') {
		++*ptr;
	} else {
		return TIMELIB_UNSET;
	}
	return retval;
}