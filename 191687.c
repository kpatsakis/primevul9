static int network_config_set_ttl (const oconfig_item_t *ci) /* {{{ */
{
  int tmp;
  if ((ci->values_num != 1)
      || (ci->values[0].type != OCONFIG_TYPE_NUMBER))
  {
    WARNING ("network plugin: The `TimeToLive' config option needs exactly "
        "one numeric argument.");
    return (-1);
  }

  tmp = (int) ci->values[0].value.number;
  if ((tmp > 0) && (tmp <= 255))
    network_config_ttl = tmp;
  else {
    WARNING ("network plugin: The `TimeToLive' must be between 1 and 255.");
    return (-1);
  }

  return (0);
} /* }}} int network_config_set_ttl */