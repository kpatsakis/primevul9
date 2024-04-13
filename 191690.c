static int write_part_number (char **ret_buffer, int *ret_buffer_len,
		int type, uint64_t value)
{
	char *packet_ptr;
	int packet_len;

	part_header_t pkg_head;
	uint64_t pkg_value;

	int offset;

	packet_len = sizeof (pkg_head) + sizeof (pkg_value);

	if (*ret_buffer_len < packet_len)
		return (-1);

	pkg_head.type = htons (type);
	pkg_head.length = htons (packet_len);
	pkg_value = htonll (value);

	packet_ptr = *ret_buffer;
	offset = 0;
	memcpy (packet_ptr + offset, &pkg_head, sizeof (pkg_head));
	offset += sizeof (pkg_head);
	memcpy (packet_ptr + offset, &pkg_value, sizeof (pkg_value));
	offset += sizeof (pkg_value);

	assert (offset == packet_len);

	*ret_buffer = packet_ptr + packet_len;
	*ret_buffer_len -= packet_len;

	return (0);
} /* int write_part_number */