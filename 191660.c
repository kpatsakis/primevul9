static int network_config (oconfig_item_t *ci) /* {{{ */
{
  int i;

  /* The options need to be applied first */
  for (i = 0; i < ci->children_num; i++)
  {
    oconfig_item_t *child = ci->children + i;
    if (strcasecmp ("TimeToLive", child->key) == 0)
      network_config_set_ttl (child);
  }

  for (i = 0; i < ci->children_num; i++)
  {
    oconfig_item_t *child = ci->children + i;

    if (strcasecmp ("Listen", child->key) == 0)
      network_config_add_listen (child);
    else if (strcasecmp ("Server", child->key) == 0)
      network_config_add_server (child);
    else if (strcasecmp ("TimeToLive", child->key) == 0) {
      /* Handled earlier */
    }
    else if (strcasecmp ("MaxPacketSize", child->key) == 0)
      network_config_set_buffer_size (child);
    else if (strcasecmp ("Forward", child->key) == 0)
      network_config_set_boolean (child, &network_config_forward);
    else if (strcasecmp ("ReportStats", child->key) == 0)
      network_config_set_boolean (child, &network_config_stats);
    else
    {
      WARNING ("network plugin: Option `%s' is not allowed here.",
          child->key);
    }
  }

  return (0);
} /* }}} int network_config */