lcc_network_buffer_t *lcc_network_buffer_create (size_t size) /* {{{ */
{
  lcc_network_buffer_t *nb;

  if (size == 0)
    size = LCC_NETWORK_BUFFER_SIZE_DEFAULT;

  if (size < 128)
  {
    errno = EINVAL;
    return (NULL);
  }

  nb = malloc (sizeof (*nb));
  if (nb == NULL)
    return (NULL);
  memset (nb, 0, sizeof (*nb));

  nb->size = size;
  nb->buffer = malloc (nb->size);
  if (nb->buffer == NULL)
  {
    free (nb);
    return (NULL);
  }
  memset (nb->buffer, 0, nb->size);

  nb->ptr = nb->buffer;
  nb->free = nb->size;

  nb->seclevel = NONE;
  nb->username = NULL;
  nb->password = NULL;

  return (nb);
} /* }}} lcc_network_buffer_t *lcc_network_buffer_create */