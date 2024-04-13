static int network_stats_read(void) /* {{{ */
{
  derive_t copy_octets_rx;
  derive_t copy_octets_tx;
  derive_t copy_packets_rx;
  derive_t copy_packets_tx;
  derive_t copy_values_dispatched;
  derive_t copy_values_not_dispatched;
  derive_t copy_values_sent;
  derive_t copy_values_not_sent;
  derive_t copy_receive_list_length;
  value_list_t vl = VALUE_LIST_INIT;
  value_t values[2];

  copy_octets_rx = stats_octets_rx;
  copy_octets_tx = stats_octets_tx;
  copy_packets_rx = stats_packets_rx;
  copy_packets_tx = stats_packets_tx;
  copy_values_dispatched = stats_values_dispatched;
  copy_values_not_dispatched = stats_values_not_dispatched;
  copy_values_sent = stats_values_sent;
  copy_values_not_sent = stats_values_not_sent;
  copy_receive_list_length = receive_list_length;

  /* Initialize `vl' */
  vl.values = values;
  vl.values_len = 2;
  vl.time = 0;
  sstrncpy(vl.host, hostname_g, sizeof(vl.host));
  sstrncpy(vl.plugin, "network", sizeof(vl.plugin));

  /* Octets received / sent */
  vl.values[0].derive = (derive_t)copy_octets_rx;
  vl.values[1].derive = (derive_t)copy_octets_tx;
  sstrncpy(vl.type, "if_octets", sizeof(vl.type));
  plugin_dispatch_values(&vl);

  /* Packets received / send */
  vl.values[0].derive = (derive_t)copy_packets_rx;
  vl.values[1].derive = (derive_t)copy_packets_tx;
  sstrncpy(vl.type, "if_packets", sizeof(vl.type));
  plugin_dispatch_values(&vl);

  /* Values (not) dispatched and (not) send */
  sstrncpy(vl.type, "total_values", sizeof(vl.type));
  vl.values_len = 1;

  vl.values[0].derive = (derive_t)copy_values_dispatched;
  sstrncpy(vl.type_instance, "dispatch-accepted", sizeof(vl.type_instance));
  plugin_dispatch_values(&vl);

  vl.values[0].derive = (derive_t)copy_values_not_dispatched;
  sstrncpy(vl.type_instance, "dispatch-rejected", sizeof(vl.type_instance));
  plugin_dispatch_values(&vl);

  vl.values[0].derive = (derive_t)copy_values_sent;
  sstrncpy(vl.type_instance, "send-accepted", sizeof(vl.type_instance));
  plugin_dispatch_values(&vl);

  vl.values[0].derive = (derive_t)copy_values_not_sent;
  sstrncpy(vl.type_instance, "send-rejected", sizeof(vl.type_instance));
  plugin_dispatch_values(&vl);

  /* Receive queue length */
  vl.values[0].gauge = (gauge_t)copy_receive_list_length;
  sstrncpy(vl.type, "queue_length", sizeof(vl.type));
  vl.type_instance[0] = 0;
  plugin_dispatch_values(&vl);

  return (0);
} /* }}} int network_stats_read */