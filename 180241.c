static _Bool check_notify_received(const notification_t *n) /* {{{ */
{
  for (notification_meta_t *ptr = n->meta; ptr != NULL; ptr = ptr->next)
    if ((strcmp("network:received", ptr->name) == 0) &&
        (ptr->type == NM_TYPE_BOOLEAN))
      return ((_Bool)ptr->nm_value.nm_boolean);

  return (0);
} /* }}} _Bool check_notify_received */