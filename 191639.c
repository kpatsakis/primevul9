int lcc_network_buffer_add_value (lcc_network_buffer_t *nb, /* {{{ */
    const lcc_value_list_t *vl)
{
  int status;

  if ((nb == NULL) || (vl == NULL))
    return (EINVAL);

  status = nb_add_value_list (nb, vl);
  return (status);
} /* }}} int lcc_network_buffer_add_value */