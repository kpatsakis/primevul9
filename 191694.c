static int network_config_set_buffer_size (const oconfig_item_t *ci) /* {{{ */
{
  int tmp;
  if ((ci->values_num != 1)
      || (ci->values[0].type != OCONFIG_TYPE_NUMBER))
  {
    WARNING ("network plugin: The `MaxPacketSize' config option needs exactly "
        "one numeric argument.");
    return (-1);
  }

  tmp = (int) ci->values[0].value.number;
  if ((tmp >= 1024) && (tmp <= 65535))
    network_config_packet_size = tmp;

  return (0);
} /* }}} int network_config_set_buffer_size */