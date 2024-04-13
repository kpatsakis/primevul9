static int parse_part_number (void **ret_buffer, size_t *ret_buffer_len,
		uint64_t *value)
{
	char *buffer = *ret_buffer;
	size_t buffer_len = *ret_buffer_len;

	uint16_t tmp16;
	uint64_t tmp64;
	size_t exp_size = 2 * sizeof (uint16_t) + sizeof (uint64_t);

	uint16_t pkg_length;

	if (buffer_len < exp_size)
	{
		WARNING ("network plugin: parse_part_number: "
				"Packet too short: "
				"Chunk of size %zu expected, "
				"but buffer has only %zu bytes left.",
				exp_size, buffer_len);
		return (-1);
	}

	memcpy ((void *) &tmp16, buffer, sizeof (tmp16));
	buffer += sizeof (tmp16);
	/* pkg_type = ntohs (tmp16); */

	memcpy ((void *) &tmp16, buffer, sizeof (tmp16));
	buffer += sizeof (tmp16);
	pkg_length = ntohs (tmp16);

	memcpy ((void *) &tmp64, buffer, sizeof (tmp64));
	buffer += sizeof (tmp64);
	*value = ntohll (tmp64);

	*ret_buffer = buffer;
	*ret_buffer_len = buffer_len - pkg_length;

	return (0);
} /* int parse_part_number */