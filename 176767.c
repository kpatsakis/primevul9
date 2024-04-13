static void cleanup_server_session(MYSQL_SESSION session,
                                   bool initialize_thread) {
  srv_session_close(session);
  if (initialize_thread) srv_session_deinit_thread();
}