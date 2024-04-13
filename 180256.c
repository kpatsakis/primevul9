static int sockent_add(sockent_t *se) /* {{{ */
{
  sockent_t *last_ptr;

  if (se == NULL)
    return (-1);

  if (se->type == SOCKENT_TYPE_SERVER) {
    struct pollfd *tmp;

    tmp = realloc(listen_sockets_pollfd,
                  sizeof(*tmp) * (listen_sockets_num + se->data.server.fd_num));
    if (tmp == NULL) {
      ERROR("network plugin: realloc failed.");
      return (-1);
    }
    listen_sockets_pollfd = tmp;
    tmp = listen_sockets_pollfd + listen_sockets_num;

    for (size_t i = 0; i < se->data.server.fd_num; i++) {
      memset(tmp + i, 0, sizeof(*tmp));
      tmp[i].fd = se->data.server.fd[i];
      tmp[i].events = POLLIN | POLLPRI;
      tmp[i].revents = 0;
    }

    listen_sockets_num += se->data.server.fd_num;

    if (listen_sockets == NULL) {
      listen_sockets = se;
      return (0);
    }
    last_ptr = listen_sockets;
  } else /* if (se->type == SOCKENT_TYPE_CLIENT) */
  {
    if (sending_sockets == NULL) {
      sending_sockets = se;
      return (0);
    }
    last_ptr = sending_sockets;
  }

  while (last_ptr->next != NULL)
    last_ptr = last_ptr->next;
  last_ptr->next = se;

  return (0);
} /* }}} int sockent_add */