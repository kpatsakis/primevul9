static int network_init(void) {
  static _Bool have_init = 0;

  /* Check if we were already initialized. If so, just return - there's
   * nothing more to do (for now, that is). */
  if (have_init)
    return (0);
  have_init = 1;

  if (network_config_stats)
    plugin_register_read("network", network_stats_read);

  plugin_register_shutdown("network", network_shutdown);

  send_buffer = malloc(network_config_packet_size);
  if (send_buffer == NULL) {
    ERROR("network plugin: malloc failed.");
    return (-1);
  }
  network_init_buffer();

  /* setup socket(s) and so on */
  if (sending_sockets != NULL) {
    plugin_register_write("network", network_write,
                          /* user_data = */ NULL);
    plugin_register_notification("network", network_notification,
                                 /* user_data = */ NULL);
  }

  /* If no threads need to be started, return here. */
  if ((listen_sockets_num == 0) ||
      ((dispatch_thread_running != 0) && (receive_thread_running != 0)))
    return (0);

  if (dispatch_thread_running == 0) {
    int status;
    status = plugin_thread_create(&dispatch_thread_id, NULL /* no attributes */,
                                  dispatch_thread, NULL /* no argument */);
    if (status != 0) {
      char errbuf[1024];
      ERROR("network: pthread_create failed: %s",
            sstrerror(errno, errbuf, sizeof(errbuf)));
    } else {
      dispatch_thread_running = 1;
    }
  }

  if (receive_thread_running == 0) {
    int status;
    status = plugin_thread_create(&receive_thread_id, NULL /* no attributes */,
                                  receive_thread, NULL /* no argument */);
    if (status != 0) {
      char errbuf[1024];
      ERROR("network: pthread_create failed: %s",
            sstrerror(errno, errbuf, sizeof(errbuf)));
    } else {
      receive_thread_running = 1;
    }
  }

  return (0);
} /* int network_init */