static int network_config_set_buffer_size(const oconfig_item_t *ci) /* {{{ */
{
  int tmp = 0;

  if (cf_util_get_int(ci, &tmp) != 0)
    return (-1);
  else if ((tmp >= 1024) && (tmp <= 65535))
    network_config_packet_size = tmp;
  else {
    WARNING(
        "network plugin: The `MaxPacketSize' must be between 1024 and 65535.");
    return (-1);
  }

  return (0);
} /* }}} int network_config_set_buffer_size */