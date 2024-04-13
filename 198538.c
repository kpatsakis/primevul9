int sc_mem_reverse(unsigned char *buf, size_t len)
{
	unsigned char ch;
	size_t ii;

	if (!buf || !len)
		return SC_ERROR_INVALID_ARGUMENTS;

	for (ii = 0; ii < len / 2; ii++)   {
		ch = *(buf + ii);
		*(buf + ii) = *(buf + len - 1 - ii);
		*(buf + len - 1 - ii) = ch;
	}

	return SC_SUCCESS;
}