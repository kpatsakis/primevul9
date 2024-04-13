static int nb_add_number (char **ret_buffer, /* {{{ */
    size_t *ret_buffer_len,
    uint16_t type, uint64_t value)
{
  char *packet_ptr;
  size_t packet_len;

  uint16_t pkg_type;
  uint16_t pkg_length;
  uint64_t pkg_value;

  size_t offset;

  packet_len = sizeof (pkg_type)
    + sizeof (pkg_length)
    + sizeof (pkg_value);

  if (*ret_buffer_len < packet_len)
    return (ENOMEM);

  pkg_type = htons (type);
  pkg_length = htons ((uint16_t) packet_len);
  pkg_value = htonll (value);

  packet_ptr = *ret_buffer;
  offset = 0;
  memcpy (packet_ptr + offset, &pkg_type, sizeof (pkg_type));
  offset += sizeof (pkg_type);
  memcpy (packet_ptr + offset, &pkg_length, sizeof (pkg_length));
  offset += sizeof (pkg_length);
  memcpy (packet_ptr + offset, &pkg_value, sizeof (pkg_value));
  offset += sizeof (pkg_value);

  assert (offset == packet_len);

  *ret_buffer = packet_ptr + packet_len;
  *ret_buffer_len -= packet_len;
  return (0);
} /* }}} int nb_add_number */