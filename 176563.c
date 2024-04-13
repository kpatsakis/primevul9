command_process_netinfo_cell(cell_t *cell, or_connection_t *conn)
{
  time_t timestamp;
  uint8_t my_addr_type;
  uint8_t my_addr_len;
  const uint8_t *my_addr_ptr;
  const uint8_t *cp, *end;
  uint8_t n_other_addrs;
  time_t now = time(NULL);

  long apparent_skew = 0;
  uint32_t my_apparent_addr = 0;

  if (conn->link_proto < 2) {
    log_fn(LOG_PROTOCOL_WARN, LD_OR,
           "Received a NETINFO cell on %s connection; dropping.",
           conn->link_proto == 0 ? "non-versioned" : "a v1");
    return;
  }
  if (conn->_base.state != OR_CONN_STATE_OR_HANDSHAKING) {
    log_fn(LOG_PROTOCOL_WARN, LD_OR,
           "Received a NETINFO cell on non-handshaking connection; dropping.");
    return;
  }
  tor_assert(conn->handshake_state &&
             conn->handshake_state->received_versions);
  /* Decode the cell. */
  timestamp = ntohl(get_uint32(cell->payload));
  if (labs(now - conn->handshake_state->sent_versions_at) < 180) {
    apparent_skew = now - timestamp;
  }

  my_addr_type = (uint8_t) cell->payload[4];
  my_addr_len = (uint8_t) cell->payload[5];
  my_addr_ptr = (uint8_t*) cell->payload + 6;
  end = cell->payload + CELL_PAYLOAD_SIZE;
  cp = cell->payload + 6 + my_addr_len;
  if (cp >= end) {
    log_fn(LOG_PROTOCOL_WARN, LD_OR,
           "Addresses too long in netinfo cell; closing connection.");
    connection_mark_for_close(TO_CONN(conn));
    return;
  } else if (my_addr_type == RESOLVED_TYPE_IPV4 && my_addr_len == 4) {
    my_apparent_addr = ntohl(get_uint32(my_addr_ptr));
  }

  n_other_addrs = (uint8_t) *cp++;
  while (n_other_addrs && cp < end-2) {
    /* Consider all the other addresses; if any matches, this connection is
     * "canonical." */
    tor_addr_t addr;
    const uint8_t *next =
      decode_address_from_payload(&addr, cp, (int)(end-cp));
    if (next == NULL) {
      log_fn(LOG_PROTOCOL_WARN,  LD_OR,
             "Bad address in netinfo cell; closing connection.");
      connection_mark_for_close(TO_CONN(conn));
      return;
    }
    if (tor_addr_eq(&addr, &conn->real_addr)) {
      conn->is_canonical = 1;
      break;
    }
    cp = next;
    --n_other_addrs;
  }

  /* Act on apparent skew. */
  /** Warn when we get a netinfo skew with at least this value. */
#define NETINFO_NOTICE_SKEW 3600
  if (labs(apparent_skew) > NETINFO_NOTICE_SKEW &&
      router_get_by_digest(conn->identity_digest)) {
    char dbuf[64];
    int severity;
    /*XXXX be smarter about when everybody says we are skewed. */
    if (router_digest_is_trusted_dir(conn->identity_digest))
      severity = LOG_WARN;
    else
      severity = LOG_INFO;
    format_time_interval(dbuf, sizeof(dbuf), apparent_skew);
    log_fn(severity, LD_GENERAL, "Received NETINFO cell with skewed time from "
           "server at %s:%d.  It seems that our clock is %s by %s, or "
           "that theirs is %s. Tor requires an accurate clock to work: "
           "please check your time and date settings.",
           conn->_base.address, (int)conn->_base.port,
           apparent_skew>0 ? "ahead" : "behind", dbuf,
           apparent_skew>0 ? "behind" : "ahead");
    if (severity == LOG_WARN) /* only tell the controller if an authority */
      control_event_general_status(LOG_WARN,
                          "CLOCK_SKEW SKEW=%ld SOURCE=OR:%s:%d",
                          apparent_skew,
                          conn->_base.address, conn->_base.port);
  }

  /* XXX maybe act on my_apparent_addr, if the source is sufficiently
   * trustworthy. */
  (void)my_apparent_addr;

  if (connection_or_set_state_open(conn)<0)
    connection_mark_for_close(TO_CONN(conn));
  else
    log_info(LD_OR, "Got good NETINFO cell from %s:%d; OR connection is now "
             "open, using protocol version %d",
             safe_str(conn->_base.address), conn->_base.port,
             (int)conn->link_proto);
  assert_connection_ok(TO_CONN(conn),time(NULL));
}