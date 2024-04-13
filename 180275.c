static int network_notification(const notification_t *n,
                                user_data_t __attribute__((unused)) *
                                    user_data) {
  char buffer[network_config_packet_size];
  char *buffer_ptr = buffer;
  size_t buffer_free = sizeof(buffer);
  int status;

  if (!check_send_notify_okay(n))
    return (0);

  memset(buffer, 0, sizeof(buffer));

  status = write_part_number(&buffer_ptr, &buffer_free, TYPE_TIME_HR,
                             (uint64_t)n->time);
  if (status != 0)
    return (-1);

  status = write_part_number(&buffer_ptr, &buffer_free, TYPE_SEVERITY,
                             (uint64_t)n->severity);
  if (status != 0)
    return (-1);

  if (strlen(n->host) > 0) {
    status = write_part_string(&buffer_ptr, &buffer_free, TYPE_HOST, n->host,
                               strlen(n->host));
    if (status != 0)
      return (-1);
  }

  if (strlen(n->plugin) > 0) {
    status = write_part_string(&buffer_ptr, &buffer_free, TYPE_PLUGIN,
                               n->plugin, strlen(n->plugin));
    if (status != 0)
      return (-1);
  }

  if (strlen(n->plugin_instance) > 0) {
    status = write_part_string(&buffer_ptr, &buffer_free, TYPE_PLUGIN_INSTANCE,
                               n->plugin_instance, strlen(n->plugin_instance));
    if (status != 0)
      return (-1);
  }

  if (strlen(n->type) > 0) {
    status = write_part_string(&buffer_ptr, &buffer_free, TYPE_TYPE, n->type,
                               strlen(n->type));
    if (status != 0)
      return (-1);
  }

  if (strlen(n->type_instance) > 0) {
    status = write_part_string(&buffer_ptr, &buffer_free, TYPE_TYPE_INSTANCE,
                               n->type_instance, strlen(n->type_instance));
    if (status != 0)
      return (-1);
  }

  status = write_part_string(&buffer_ptr, &buffer_free, TYPE_MESSAGE,
                             n->message, strlen(n->message));
  if (status != 0)
    return (-1);

  network_send_buffer(buffer, sizeof(buffer) - buffer_free);

  return (0);
} /* int network_notification */