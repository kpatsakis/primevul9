static int network_config_add_server (const oconfig_item_t *ci) /* {{{ */
{
  sockent_t *se;
  int status;
  int i;

  if ((ci->values_num < 1) || (ci->values_num > 2)
      || (ci->values[0].type != OCONFIG_TYPE_STRING)
      || ((ci->values_num > 1) && (ci->values[1].type != OCONFIG_TYPE_STRING)))
  {
    ERROR ("network plugin: The `%s' config option needs "
        "one or two string arguments.", ci->key);
    return (-1);
  }

  se = sockent_create (SOCKENT_TYPE_CLIENT);
  if (se == NULL)
  {
    ERROR ("network plugin: sockent_create failed.");
    return (-1);
  }

  se->node = strdup (ci->values[0].value.string);
  if (ci->values_num >= 2)
    se->service = strdup (ci->values[1].value.string);

  for (i = 0; i < ci->children_num; i++)
  {
    oconfig_item_t *child = ci->children + i;

#if HAVE_LIBGCRYPT
    if (strcasecmp ("Username", child->key) == 0)
      network_config_set_string (child, &se->data.client.username);
    else if (strcasecmp ("Password", child->key) == 0)
      network_config_set_string (child, &se->data.client.password);
    else if (strcasecmp ("SecurityLevel", child->key) == 0)
      network_config_set_security_level (child,
          &se->data.client.security_level);
    else
#endif /* HAVE_LIBGCRYPT */
    if (strcasecmp ("Interface", child->key) == 0)
      network_config_set_interface (child,
          &se->interface);
    else
    {
      WARNING ("network plugin: Option `%s' is not allowed here.",
          child->key);
    }
  }

#if HAVE_LIBGCRYPT
  if ((se->data.client.security_level > SECURITY_LEVEL_NONE)
      && ((se->data.client.username == NULL)
        || (se->data.client.password == NULL)))
  {
    ERROR ("network plugin: A security level higher than `none' was "
        "requested, but no Username or Password option was given. "
        "Cowardly refusing to open this socket!");
    sockent_destroy (se);
    return (-1);
  }
#endif /* HAVE_LIBGCRYPT */

  status = sockent_init_crypto (se);
  if (status != 0)
  {
    ERROR ("network plugin: network_config_add_server: sockent_init_crypto() failed.");
    sockent_destroy (se);
    return (-1);
  }

  /* No call to sockent_client_connect() here -- it is called from
   * networt_send_buffer_plain(). */

  status = sockent_add (se);
  if (status != 0)
  {
    ERROR ("network plugin: network_config_add_server: sockent_add failed.");
    sockent_destroy (se);
    return (-1);
  }

  return (0);
} /* }}} int network_config_add_server */