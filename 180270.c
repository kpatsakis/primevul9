static _Bool check_send_notify_okay(const notification_t *n) /* {{{ */
{
  static c_complain_t complain_forwarding = C_COMPLAIN_INIT_STATIC;
  _Bool received = 0;

  if (n->meta == NULL)
    return (1);

  received = check_notify_received(n);

  if (network_config_forward && received) {
    c_complain_once(
        LOG_ERR, &complain_forwarding,
        "network plugin: A notification has been received via the network "
        "and forwarding is enabled. Forwarding of notifications is currently "
        "not supported, because there is not loop-deteciton available. "
        "Please contact the collectd mailing list if you need this "
        "feature.");
  }

  /* By default, only *send* value lists that were not *received* by the
   * network plugin. */
  return (!received);
} /* }}} _Bool check_send_notify_okay */