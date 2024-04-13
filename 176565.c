command_process_versions_cell(var_cell_t *cell, or_connection_t *conn)
{
  int highest_supported_version = 0;
  const uint8_t *cp, *end;
  if (conn->link_proto != 0 ||
      conn->_base.state != OR_CONN_STATE_OR_HANDSHAKING ||
      (conn->handshake_state && conn->handshake_state->received_versions)) {
    log_fn(LOG_PROTOCOL_WARN, LD_OR,
           "Received a VERSIONS cell on a connection with its version "
           "already set to %d; dropping", (int) conn->link_proto);
    return;
  }
  tor_assert(conn->handshake_state);
  end = cell->payload + cell->payload_len;
  for (cp = cell->payload; cp+1 < end; ++cp) {
    uint16_t v = ntohs(get_uint16(cp));
    if (is_or_protocol_version_known(v) && v > highest_supported_version)
      highest_supported_version = v;
  }
  if (!highest_supported_version) {
    log_fn(LOG_PROTOCOL_WARN, LD_OR,
           "Couldn't find a version in common between my version list and the "
           "list in the VERSIONS cell; closing connection.");
    connection_mark_for_close(TO_CONN(conn));
    return;
  } else if (highest_supported_version == 1) {
    /* Negotiating version 1 makes no sense, since version 1 has no VERSIONS
     * cells. */
    log_fn(LOG_PROTOCOL_WARN, LD_OR,
           "Used version negotiation protocol to negotiate a v1 connection. "
           "That's crazily non-compliant. Closing connection.");
    connection_mark_for_close(TO_CONN(conn));
    return;
  }
  conn->link_proto = highest_supported_version;
  conn->handshake_state->received_versions = 1;

  log_info(LD_OR, "Negotiated version %d with %s:%d; sending NETINFO.",
           highest_supported_version, safe_str(conn->_base.address),
           conn->_base.port);
  tor_assert(conn->link_proto >= 2);

  if (connection_or_send_netinfo(conn) < 0) {
    connection_mark_for_close(TO_CONN(conn));
    return;
  }
}