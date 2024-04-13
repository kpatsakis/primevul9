std::string wsrep_sst_prepare() {
  const ssize_t ip_max = 256;
  char ip_buf[ip_max];
  const char *addr_in = NULL;
  const char *addr_out = NULL;
  const char *method;

  if (!strcmp(wsrep_sst_method, WSREP_SST_SKIP)) {
    return WSREP_STATE_TRANSFER_TRIVIAL;
  }

  // Figure out SST address. Common for all SST methods
  if (wsrep_sst_receive_address &&
      strcmp(wsrep_sst_receive_address, WSREP_SST_ADDRESS_AUTO)) {
    addr_in = wsrep_sst_receive_address;
  } else if (wsrep_node_address && strlen(wsrep_node_address)) {
    size_t const addr_len = strlen(wsrep_node_address);
    size_t const host_len = wsrep_host_len(wsrep_node_address, addr_len);

    if (host_len < addr_len) {
      strncpy(ip_buf, wsrep_node_address, host_len);
      ip_buf[host_len] = '\0';
      addr_in = ip_buf;
    } else {
      addr_in = wsrep_node_address;
    }
  } else {
    ssize_t ret = wsrep_guess_ip(ip_buf, ip_max);

    if (ret && ret < ip_max) {
      addr_in = ip_buf;
    } else {
      WSREP_ERROR(
          "Could not prepare state transfer request: "
          "failed to guess address to accept state transfer at. "
          "wsrep_sst_receive_address must be set manually.");
      throw wsrep::runtime_error("Could not prepare state transfer request");
    }
  }

  ssize_t addr_len = -ENOSYS;
  method = wsrep_sst_method;
  if (Wsrep_server_state::instance().is_initialized() &&
      Wsrep_server_state::instance().state() == Wsrep_server_state::s_joiner) {
    // we already did SST at initializaiton, now engines are running
    WSREP_INFO(
        "WSREP: "
        "You have configured '%s' state snapshot transfer method "
        "which cannot be performed on a running server. "
        "Wsrep provider won't be able to fall back to it "
        "if other means of state transfer are unavailable. "
        "In that case you will need to restart the server.",
        wsrep_sst_method);
    return "";
  }

  addr_len = sst_prepare_other(wsrep_sst_method, addr_in, &addr_out);
  if (addr_len < 0) {
    WSREP_ERROR("Failed to prepare for '%s' SST. Unrecoverable.",
                wsrep_sst_method);
    throw wsrep::runtime_error("Failed to prepare for SST. Unrecoverable");
  }

  std::string ret;
  ret += method;
  ret.push_back('\0');
  ret += addr_out;

  const char *method_ptr(ret.data());
  const char *addr_ptr(ret.data() + strlen(method_ptr) + 1);
  WSREP_INFO("Prepared SST request: %s|%s", method_ptr, addr_ptr);

  if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
  sst_awaiting_callback = true;
  mysql_mutex_unlock(&LOCK_wsrep_sst);

  if (addr_out != addr_in) /* malloc'ed */
    free(const_cast<char *>(addr_out));

  return ret;
}