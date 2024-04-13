static void sockent_destroy (sockent_t *se) /* {{{ */
{
  sockent_t *next;

  DEBUG ("network plugin: sockent_destroy (se = %p);", (void *) se);

  while (se != NULL)
  {
    next = se->next;

    sfree (se->node);
    sfree (se->service);

    if (se->type == SOCKENT_TYPE_CLIENT)
      free_sockent_client (&se->data.client);
    else
      free_sockent_server (&se->data.server);

    sfree (se);
    se = next;
  }
} /* }}} void sockent_destroy */