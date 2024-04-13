static int run_sql_command(THD *thd, const char *query,
                           const char *safe_query) {
  thd->set_query(query, strlen(query));

  Parser_state ps;
  if (ps.init(thd, thd->query().str, thd->query().length)) {
    WSREP_ERROR("SST query: %s failed", (safe_query ? safe_query : query));
    return -1;
  }

  mysql_parse(thd, &ps, false);
  if (thd->is_error()) {
    int const err = thd->get_stmt_da()->mysql_errno();
    if (safe_query) {
      WSREP_WARN("error executing '%s' : %d", safe_query, err);
    } else {
      WSREP_WARN("error executing '%s' : %d (%s)", query, err,
                 thd->get_stmt_da()->message_text());
    }
    thd->clear_error();
    return err;
  }
  return 0;
}