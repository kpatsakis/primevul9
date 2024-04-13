static int nb_add_values (char **ret_buffer, /* {{{ */
    size_t *ret_buffer_len,
    const lcc_value_list_t *vl)
{
  char *packet_ptr;
  size_t packet_len;

  uint16_t      pkg_type;
  uint16_t      pkg_length;
  uint16_t      pkg_num_values;
  uint8_t       pkg_values_types[vl->values_len];
  value_t       pkg_values[vl->values_len];

  size_t offset;
  size_t i;

  packet_len = sizeof (pkg_type) + sizeof (pkg_length)
    + sizeof (pkg_num_values)
    + sizeof (pkg_values_types)
    + sizeof (pkg_values);

  if (*ret_buffer_len < packet_len)
    return (ENOMEM);

  pkg_type = htons (TYPE_VALUES);
  pkg_length = htons ((uint16_t) packet_len);
  pkg_num_values = htons ((uint16_t) vl->values_len);

  for (i = 0; i < vl->values_len; i++)
  {
    pkg_values_types[i] = (uint8_t) vl->values_types[i];
    switch (vl->values_types[i])
    {
      case LCC_TYPE_COUNTER:
        pkg_values[i].counter = (counter_t) htonll (vl->values[i].counter);
        break;

      case LCC_TYPE_GAUGE:
        pkg_values[i].gauge = (gauge_t) htond (vl->values[i].gauge);
        break;

      case LCC_TYPE_DERIVE:
        pkg_values[i].derive = (derive_t) htonll (vl->values[i].derive);
        break;

      case LCC_TYPE_ABSOLUTE:
        pkg_values[i].absolute = (absolute_t) htonll (vl->values[i].absolute);
        break;

      default:
        return (EINVAL);
    } /* switch (vl->values_types[i]) */
  } /* for (vl->values_len) */

  /*
   * Use `memcpy' to write everything to the buffer, because the pointer
   * may be unaligned and some architectures, such as SPARC, can't handle
   * that.
   */
  packet_ptr = *ret_buffer;
  offset = 0;
  memcpy (packet_ptr + offset, &pkg_type, sizeof (pkg_type));
  offset += sizeof (pkg_type);
  memcpy (packet_ptr + offset, &pkg_length, sizeof (pkg_length));
  offset += sizeof (pkg_length);
  memcpy (packet_ptr + offset, &pkg_num_values, sizeof (pkg_num_values));
  offset += sizeof (pkg_num_values);
  memcpy (packet_ptr + offset, pkg_values_types, sizeof (pkg_values_types));
  offset += sizeof (pkg_values_types);
  memcpy (packet_ptr + offset, pkg_values, sizeof (pkg_values));
  offset += sizeof (pkg_values);

  assert (offset == packet_len);

  *ret_buffer = packet_ptr + packet_len;
  *ret_buffer_len -= packet_len;
  return (0);
} /* }}} int nb_add_values */