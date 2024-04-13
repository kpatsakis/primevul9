static int network_dispatch_notification(notification_t *n) /* {{{ */
{
  int status;

  assert(n->meta == NULL);

  status = plugin_notification_meta_add_boolean(n, "network:received", 1);
  if (status != 0) {
    ERROR("network plugin: plugin_notification_meta_add_boolean failed.");
    plugin_notification_meta_free(n->meta);
    n->meta = NULL;
    return (status);
  }

  status = plugin_dispatch_notification(n);

  plugin_notification_meta_free(n->meta);
  n->meta = NULL;

  return (status);
} /* }}} int network_dispatch_notification */