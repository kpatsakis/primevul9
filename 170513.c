static int phar_tar_octal(char *buf, uint32_t val, int len) /* {{{ */
{
	char *p = buf;
	int s = len;

	p += len;		/* Start at the end and work backwards. */
	while (s-- > 0) {
		*--p = (char)('0' + (val & 7));
		val >>= 3;
	}

	if (val == 0)
		return SUCCESS;

	/* If it overflowed, fill field with max value. */
	while (len-- > 0)
		*p++ = '7';

	return FAILURE;
}