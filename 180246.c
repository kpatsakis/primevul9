static _Bool check_send_okay(const value_list_t *vl) /* {{{ */
{
  _Bool received = 0;
  int status;

  if (network_config_forward)
    return (1);

  if (vl->meta == NULL)
    return (1);

  status = meta_data_get_boolean(vl->meta, "network:received", &received);
  if (status == -ENOENT)
    return (1);
  else if (status != 0) {
    ERROR("network plugin: check_send_okay: meta_data_get_boolean failed "
          "with status %i.",
          status);
    return (1);
  }

  /* By default, only *send* value lists that were not *received* by the
   * network plugin. */
  return (!received);
} /* }}} _Bool check_send_okay */