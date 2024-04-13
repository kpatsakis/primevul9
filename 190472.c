static void test_wl5968()
{
  int rc;

  myheader("test_wl5968");

  rc= mysql_query(mysql, "START TRANSACTION");
  myquery(rc);
  DIE_UNLESS(mysql->server_status & SERVER_STATUS_IN_TRANS);
  DIE_UNLESS(!(mysql->server_status & SERVER_STATUS_IN_TRANS_READONLY));
  rc= mysql_query(mysql, "COMMIT");
  myquery(rc);
  rc= mysql_query(mysql, "START TRANSACTION READ ONLY");
  myquery(rc);
  DIE_UNLESS(mysql->server_status & SERVER_STATUS_IN_TRANS);
  DIE_UNLESS(mysql->server_status & SERVER_STATUS_IN_TRANS_READONLY);
  rc= mysql_query(mysql, "COMMIT");
  myquery(rc);
  DIE_UNLESS(!(mysql->server_status & SERVER_STATUS_IN_TRANS));
  DIE_UNLESS(!(mysql->server_status & SERVER_STATUS_IN_TRANS_READONLY));
  rc= mysql_query(mysql, "START TRANSACTION");
  myquery(rc);
  DIE_UNLESS(mysql->server_status & SERVER_STATUS_IN_TRANS);
  DIE_UNLESS(!(mysql->server_status & SERVER_STATUS_IN_TRANS_READONLY));
  rc= mysql_query(mysql, "COMMIT");
  myquery(rc);
}