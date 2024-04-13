static int network_dispatch_values(value_list_t *vl, /* {{{ */
                                   const char *username) {
  int status;

  if ((vl->time == 0) || (strlen(vl->host) == 0) || (strlen(vl->plugin) == 0) ||
      (strlen(vl->type) == 0))
    return (-EINVAL);

  if (!check_receive_okay(vl)) {
#if COLLECT_DEBUG
    char name[6 * DATA_MAX_NAME_LEN];
    FORMAT_VL(name, sizeof(name), vl);
    name[sizeof(name) - 1] = 0;
    DEBUG("network plugin: network_dispatch_values: "
          "NOT dispatching %s.",
          name);
#endif
    stats_values_not_dispatched++;
    return (0);
  }

  assert(vl->meta == NULL);

  vl->meta = meta_data_create();
  if (vl->meta == NULL) {
    ERROR("network plugin: meta_data_create failed.");
    return (-ENOMEM);
  }

  status = meta_data_add_boolean(vl->meta, "network:received", 1);
  if (status != 0) {
    ERROR("network plugin: meta_data_add_boolean failed.");
    meta_data_destroy(vl->meta);
    vl->meta = NULL;
    return (status);
  }

  if (username != NULL) {
    status = meta_data_add_string(vl->meta, "network:username", username);
    if (status != 0) {
      ERROR("network plugin: meta_data_add_string failed.");
      meta_data_destroy(vl->meta);
      vl->meta = NULL;
      return (status);
    }
  }

  plugin_dispatch_values(vl);
  stats_values_dispatched++;

  meta_data_destroy(vl->meta);
  vl->meta = NULL;

  return (0);
} /* }}} int network_dispatch_values */