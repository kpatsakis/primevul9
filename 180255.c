static int network_flush(cdtime_t timeout,
                         __attribute__((unused)) const char *identifier,
                         __attribute__((unused)) user_data_t *user_data) {
  pthread_mutex_lock(&send_buffer_lock);

  if (send_buffer_fill > 0) {
    if (timeout > 0) {
      cdtime_t now = cdtime();
      if ((send_buffer_last_update + timeout) > now) {
        pthread_mutex_unlock(&send_buffer_lock);
        return (0);
      }
    }
    flush_buffer();
  }
  pthread_mutex_unlock(&send_buffer_lock);

  return (0);
} /* int network_flush */