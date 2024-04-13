static int nb_add_time (char **ret_buffer, /* {{{ */
    size_t *ret_buffer_len,
    uint16_t type, double value)
{
  /* Convert to collectd's "cdtime" representation. */
  uint64_t cdtime_value = (uint64_t) (value * 1073741824.0);
  return (nb_add_number (ret_buffer, ret_buffer_len, type, cdtime_value));
} /* }}} int nb_add_time */