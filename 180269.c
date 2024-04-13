static int network_config_set_ttl(const oconfig_item_t *ci) /* {{{ */
{
  int tmp = 0;

  if (cf_util_get_int(ci, &tmp) != 0)
    return (-1);
  else if ((tmp > 0) && (tmp <= 255))
    network_config_ttl = tmp;
  else {
    WARNING("network plugin: The `TimeToLive' must be between 1 and 255.");
    return (-1);
  }

  return (0);
} /* }}} int network_config_set_ttl */