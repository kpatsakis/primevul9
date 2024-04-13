static int add_to_buffer(char *buffer, size_t buffer_size, /* {{{ */
                         value_list_t *vl_def, const data_set_t *ds,
                         const value_list_t *vl) {
  char *buffer_orig = buffer;

  if (strcmp(vl_def->host, vl->host) != 0) {
    if (write_part_string(&buffer, &buffer_size, TYPE_HOST, vl->host,
                          strlen(vl->host)) != 0)
      return (-1);
    sstrncpy(vl_def->host, vl->host, sizeof(vl_def->host));
  }

  if (vl_def->time != vl->time) {
    if (write_part_number(&buffer, &buffer_size, TYPE_TIME_HR,
                          (uint64_t)vl->time))
      return (-1);
    vl_def->time = vl->time;
  }

  if (vl_def->interval != vl->interval) {
    if (write_part_number(&buffer, &buffer_size, TYPE_INTERVAL_HR,
                          (uint64_t)vl->interval))
      return (-1);
    vl_def->interval = vl->interval;
  }

  if (strcmp(vl_def->plugin, vl->plugin) != 0) {
    if (write_part_string(&buffer, &buffer_size, TYPE_PLUGIN, vl->plugin,
                          strlen(vl->plugin)) != 0)
      return (-1);
    sstrncpy(vl_def->plugin, vl->plugin, sizeof(vl_def->plugin));
  }

  if (strcmp(vl_def->plugin_instance, vl->plugin_instance) != 0) {
    if (write_part_string(&buffer, &buffer_size, TYPE_PLUGIN_INSTANCE,
                          vl->plugin_instance,
                          strlen(vl->plugin_instance)) != 0)
      return (-1);
    sstrncpy(vl_def->plugin_instance, vl->plugin_instance,
             sizeof(vl_def->plugin_instance));
  }

  if (strcmp(vl_def->type, vl->type) != 0) {
    if (write_part_string(&buffer, &buffer_size, TYPE_TYPE, vl->type,
                          strlen(vl->type)) != 0)
      return (-1);
    sstrncpy(vl_def->type, ds->type, sizeof(vl_def->type));
  }

  if (strcmp(vl_def->type_instance, vl->type_instance) != 0) {
    if (write_part_string(&buffer, &buffer_size, TYPE_TYPE_INSTANCE,
                          vl->type_instance, strlen(vl->type_instance)) != 0)
      return (-1);
    sstrncpy(vl_def->type_instance, vl->type_instance,
             sizeof(vl_def->type_instance));
  }

  if (write_part_values(&buffer, &buffer_size, ds, vl) != 0)
    return (-1);

  return (buffer - buffer_orig);
} /* }}} int add_to_buffer */