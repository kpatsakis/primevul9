static MYSQL_SESSION setup_server_session(bool initialize_thread) {
  MYSQL_SESSION session = NULL;

  if (initialize_thread) {
    if (srv_session_init_thread(NULL)) {
      WSREP_ERROR("Failed to initialize the server session thread.");
      return NULL;
    }
  }

  session = srv_session_open(srv_session_error_handler, NULL);

  if (session == NULL) {
    if (initialize_thread) srv_session_deinit_thread();
    WSREP_ERROR("Failed to open a session for the SST commands");
    return NULL;
  }

  MYSQL_SECURITY_CONTEXT sc;
  if (thd_get_security_context(srv_session_info_get_thd(session), &sc)) {
    cleanup_server_session(session, initialize_thread);
    WSREP_ERROR(
        "Failed to fetch the security context when contacting the server");
    return NULL;
  }
  if (security_context_lookup(sc, "mysql.pxc.internal.session", "localhost",
                              NULL, NULL)) {
    cleanup_server_session(session, initialize_thread);
    WSREP_ERROR("Error accessing server with user:mysql.pxc.internal.session");
    return NULL;
  }
  // Turn wsrep off here (because the server session has it's own THD object)
  session->get_thd()->variables.wsrep_on = false;
  return session;
}