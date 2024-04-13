static uint server_session_execute(MYSQL_SESSION session, std::string query,
                                   const char *safe_query,
                                   bool ignore_error = false) {
  COM_DATA cmd;
  wsp::Sql_resultset rset;
  cmd.com_query.query = query.c_str();
  cmd.com_query.length = static_cast<unsigned int>(query.length());
  wsp::Sql_service_context_base *ctx = new wsp::Sql_service_context(&rset);
  uint err(0);

  /* execute sql command */
  command_service_run_command(
      session, COM_QUERY, &cmd, &my_charset_utf8_general_ci,
      &wsp::Sql_service_context_base::sql_service_callbacks,
      CS_TEXT_REPRESENTATION, ctx);
  delete ctx;

  err = rset.sql_errno();
  if (err && !ignore_error) {
    // an error occurred, retrieve the status/message
    if (safe_query) {
      WSREP_ERROR("Command execution failed (%d) : %s", err, safe_query);
    } else {
      WSREP_ERROR("Command execution failed (%d) : %s : %s", err,
                  rset.err_msg().c_str(), query.c_str());
    }
  }
  return err;
}