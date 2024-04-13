static int network_config_set_interface(const oconfig_item_t *ci, /* {{{ */
                                        int *interface) {
  char if_name[256];

  if (cf_util_get_string_buffer(ci, if_name, sizeof(if_name)) != 0)
    return (-1);

  *interface = if_nametoindex(if_name);
  return (0);
} /* }}} int network_config_set_interface */