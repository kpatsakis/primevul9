static void *dispatch_thread(void __attribute__((unused)) * arg) /* {{{ */
{
  while (42) {
    receive_list_entry_t *ent;
    sockent_t *se;

    /* Lock and wait for more data to come in */
    pthread_mutex_lock(&receive_list_lock);
    while ((listen_loop == 0) && (receive_list_head == NULL))
      pthread_cond_wait(&receive_list_cond, &receive_list_lock);

    /* Remove the head entry and unlock */
    ent = receive_list_head;
    if (ent != NULL)
      receive_list_head = ent->next;
    receive_list_length--;
    pthread_mutex_unlock(&receive_list_lock);

    /* Check whether we are supposed to exit. We do NOT check `listen_loop'
     * because we dispatch all missing packets before shutting down. */
    if (ent == NULL)
      break;

    /* Look for the correct `sockent_t' */
    se = listen_sockets;
    while (se != NULL) {
      size_t i;

      for (i = 0; i < se->data.server.fd_num; i++)
        if (se->data.server.fd[i] == ent->fd)
          break;

      if (i < se->data.server.fd_num)
        break;

      se = se->next;
    }

    if (se == NULL) {
      ERROR("network plugin: Got packet from FD %i, but can't "
            "find an appropriate socket entry.",
            ent->fd);
      sfree(ent->data);
      sfree(ent);
      continue;
    }

    parse_packet(se, ent->data, ent->data_len, /* flags = */ 0,
                 /* username = */ NULL);
    sfree(ent->data);
    sfree(ent);
  } /* while (42) */

  return (NULL);
} /* }}} void *dispatch_thread */