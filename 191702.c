static int write_part_string (char **ret_buffer, int *ret_buffer_len,
		int type, const char *str, int str_len)
{
	char *buffer;
	int buffer_len;

	uint16_t pkg_type;
	uint16_t pkg_length;

	int offset;

	buffer_len = 2 * sizeof (uint16_t) + str_len + 1;
	if (*ret_buffer_len < buffer_len)
		return (-1);

	pkg_type = htons (type);
	pkg_length = htons (buffer_len);

	buffer = *ret_buffer;
	offset = 0;
	memcpy (buffer + offset, (void *) &pkg_type, sizeof (pkg_type));
	offset += sizeof (pkg_type);
	memcpy (buffer + offset, (void *) &pkg_length, sizeof (pkg_length));
	offset += sizeof (pkg_length);
	memcpy (buffer + offset, str, str_len);
	offset += str_len;
	memset (buffer + offset, '\0', 1);
	offset += 1;

	assert (offset == buffer_len);

	*ret_buffer = buffer + buffer_len;
	*ret_buffer_len -= buffer_len;

	return (0);
} /* int write_part_string */