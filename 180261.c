static int network_config_add_listen(const oconfig_item_t *ci) /* {{{ */
{
  sockent_t *se;
  int status;

  if ((ci->values_num < 1) || (ci->values_num > 2) ||
      (ci->values[0].type != OCONFIG_TYPE_STRING) ||
      ((ci->values_num > 1) && (ci->values[1].type != OCONFIG_TYPE_STRING))) {
    ERROR("network plugin: The `%s' config option needs "
          "one or two string arguments.",
          ci->key);
    return (-1);
  }

  se = sockent_create(SOCKENT_TYPE_SERVER);
  if (se == NULL) {
    ERROR("network plugin: sockent_create failed.");
    return (-1);
  }

  se->node = strdup(ci->values[0].value.string);
  if (ci->values_num >= 2)
    se->service = strdup(ci->values[1].value.string);

  for (int i = 0; i < ci->children_num; i++) {
    oconfig_item_t *child = ci->children + i;

#if HAVE_LIBGCRYPT
    if (strcasecmp("AuthFile", child->key) == 0)
      cf_util_get_string(child, &se->data.server.auth_file);
    else if (strcasecmp("SecurityLevel", child->key) == 0)
      network_config_set_security_level(child, &se->data.server.security_level);
    else
#endif /* HAVE_LIBGCRYPT */
        if (strcasecmp("Interface", child->key) == 0)
      network_config_set_interface(child, &se->interface);
    else {
      WARNING("network plugin: Option `%s' is not allowed here.", child->key);
    }
  }

#if HAVE_LIBGCRYPT
  if ((se->data.server.security_level > SECURITY_LEVEL_NONE) &&
      (se->data.server.auth_file == NULL)) {
    ERROR("network plugin: A security level higher than `none' was "
          "requested, but no AuthFile option was given. Cowardly refusing to "
          "open this socket!");
    sockent_destroy(se);
    return (-1);
  }
#endif /* HAVE_LIBGCRYPT */

  status = sockent_init_crypto(se);
  if (status != 0) {
    ERROR("network plugin: network_config_add_listen: sockent_init_crypto() "
          "failed.");
    sockent_destroy(se);
    return (-1);
  }

  status = sockent_server_listen(se);
  if (status != 0) {
    ERROR("network plugin: network_config_add_listen: sockent_server_listen "
          "failed.");
    sockent_destroy(se);
    return (-1);
  }

  status = sockent_add(se);
  if (status != 0) {
    ERROR("network plugin: network_config_add_listen: sockent_add failed.");
    sockent_destroy(se);
    return (-1);
  }

  return (0);
} /* }}} int network_config_add_listen */