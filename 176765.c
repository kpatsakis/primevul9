static void sst_disallow_writes(THD *thd, bool yes) {
  /* rsync takes a snapshot. All objects of snapshot has to be consistent
  at given point in time. If say n objects are being backed up and if rsync
  reads state = at-timestamp(X) of object-1 and state = at-timestamp(Y) of
  object-2 then rsync will not work. This co-ordination is achieved by blocking
  writes in InnoDB. Note: FLUSH TABLE WITH READ LOCK will block external action
  but internal ongoing action needs to be blocked too. */
  char query_str[128] = {
      0,
  };
  ssize_t const query_max = sizeof(query_str) - 1;
  snprintf(query_str, query_max, "SET GLOBAL innodb_disallow_writes=%s",
           yes ? "true" : "false");

  if (run_sql_command(thd, query_str, NULL)) {
    WSREP_ERROR("Failed to disallow InnoDB writes");
  }
  return;
}