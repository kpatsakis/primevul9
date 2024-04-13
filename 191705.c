int lcc_network_buffer_get (lcc_network_buffer_t *nb, /* {{{ */
    void *buffer, size_t *buffer_size)
{
  size_t sz_required;
  size_t sz_available;

  if ((nb == NULL) || (buffer_size == NULL))
    return (EINVAL);

  assert (nb->size >= nb->free);
  sz_required = nb->size - nb->free;
  sz_available = *buffer_size;

  *buffer_size = sz_required;
  if (buffer != NULL)
    memcpy (buffer, nb->buffer,
        (sz_available < sz_required) ? sz_available : sz_required);

  return (0);
} /* }}} int lcc_network_buffer_get */