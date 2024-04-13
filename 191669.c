void lcc_network_buffer_destroy (lcc_network_buffer_t *nb) /* {{{ */
{
  if (nb == NULL)
    return;

  free (nb->buffer);
  free (nb);
} /* }}} void lcc_network_buffer_destroy */