static int sockent_client_disconnect(sockent_t *se) /* {{{ */
{
  struct sockent_client *client;

  if ((se == NULL) || (se->type != SOCKENT_TYPE_CLIENT))
    return (EINVAL);

  client = &se->data.client;
  if (client->fd >= 0) /* connected */
  {
    close(client->fd);
    client->fd = -1;
  }

  sfree(client->addr);
  client->addrlen = 0;

  return (0);
} /* }}} int sockent_client_disconnect */