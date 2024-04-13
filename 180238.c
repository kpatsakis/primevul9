static int network_shutdown(void) {
  listen_loop++;

  /* Kill the listening thread */
  if (receive_thread_running != 0) {
    INFO("network plugin: Stopping receive thread.");
    pthread_kill(receive_thread_id, SIGTERM);
    pthread_join(receive_thread_id, NULL /* no return value */);
    memset(&receive_thread_id, 0, sizeof(receive_thread_id));
    receive_thread_running = 0;
  }

  /* Shutdown the dispatching thread */
  if (dispatch_thread_running != 0) {
    INFO("network plugin: Stopping dispatch thread.");
    pthread_mutex_lock(&receive_list_lock);
    pthread_cond_broadcast(&receive_list_cond);
    pthread_mutex_unlock(&receive_list_lock);
    pthread_join(dispatch_thread_id, /* ret = */ NULL);
    dispatch_thread_running = 0;
  }

  sockent_destroy(listen_sockets);

  if (send_buffer_fill > 0)
    flush_buffer();

  sfree(send_buffer);

  for (sockent_t *se = sending_sockets; se != NULL; se = se->next)
    sockent_client_disconnect(se);
  sockent_destroy(sending_sockets);

  plugin_unregister_config("network");
  plugin_unregister_init("network");
  plugin_unregister_write("network");
  plugin_unregister_shutdown("network");

  return (0);
} /* int network_shutdown */