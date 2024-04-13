static void srv_session_error_handler(void *ctx MY_ATTRIBUTE((unused)),
                                      unsigned int sql_errno,
                                      const char *err_msg) {
  switch (sql_errno) {
    case ER_CON_COUNT_ERROR:
      WSREP_ERROR(
          "Can't establish an internal server connection to "
          "execute operations since the server "
          "does not have available connections, please "
          "increase @@GLOBAL.MAX_CONNECTIONS. Server error: %i.",
          sql_errno);
      break;
    default:
      WSREP_ERROR(
          "Can't establish an internal server connection to "
          "execute operations. Server error: %i. "
          "Server error message: %s",
          sql_errno, err_msg);
  }
}