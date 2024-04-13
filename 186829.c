int mysql_db_reconnect(SV* h)
{
  dTHX;
  D_imp_xxh(h);
  imp_dbh_t* imp_dbh;
  MYSQL save_socket;

  if (DBIc_TYPE(imp_xxh) == DBIt_ST)
  {
    imp_dbh = (imp_dbh_t*) DBIc_PARENT_COM(imp_xxh);
    h = DBIc_PARENT_H(imp_xxh);
  }
  else
    imp_dbh= (imp_dbh_t*) imp_xxh;

  if (mysql_errno(imp_dbh->pmysql) != CR_SERVER_GONE_ERROR &&
          mysql_errno(imp_dbh->pmysql) != CR_SERVER_LOST)
    /* Other error */
    return FALSE;

  if (!DBIc_has(imp_dbh, DBIcf_AutoCommit) || !imp_dbh->auto_reconnect)
  {
    /* We never reconnect if AutoCommit is turned off.
     * Otherwise we might get an inconsistent transaction
     * state.
     */
    return FALSE;
  }

  /* my_login will blow away imp_dbh->mysql so we save a copy of
   * imp_dbh->mysql and put it back where it belongs if the reconnect
   * fail.  Think server is down & reconnect fails but the application eval{}s
   * the execute, so next time $dbh->quote() gets called, instant SIGSEGV!
   */
  save_socket= *(imp_dbh->pmysql);
  memcpy (&save_socket, imp_dbh->pmysql,sizeof(save_socket));
  memset (imp_dbh->pmysql,0,sizeof(*(imp_dbh->pmysql)));

  /* we should disconnect the db handle before reconnecting, this will
   * prevent my_login from thinking it's adopting an active child which
   * would prevent the handle from actually reconnecting
   */
  if (!dbd_db_disconnect(h, imp_dbh) || !my_login(aTHX_ h, imp_dbh))
  {
    do_error(h, mysql_errno(imp_dbh->pmysql), mysql_error(imp_dbh->pmysql),
             mysql_sqlstate(imp_dbh->pmysql));
    memcpy (imp_dbh->pmysql, &save_socket, sizeof(save_socket));
    ++imp_dbh->stats.auto_reconnects_failed;
    return FALSE;
  }

  /*
   *  Tell DBI, that dbh->disconnect should be called for this handle
   */
  DBIc_ACTIVE_on(imp_dbh);

  ++imp_dbh->stats.auto_reconnects_ok;
  return TRUE;
}