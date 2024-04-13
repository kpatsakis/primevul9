uint64_to_str_back_len(char *ptr, guint64 value, int len)
{
	char *new_ptr;

	new_ptr = uint64_to_str_back(ptr, value);

	/* substract from len number of generated characters */
	len -= (int)(ptr - new_ptr);

	/* pad remaining with '0' */
	while (len > 0)
	{
		*(--new_ptr) = '0';
		len--;
	}

	return new_ptr;
}