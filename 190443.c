static void test_bug17512527()
{
  MYSQL *conn1, *conn2;
  MYSQL_STMT *stmt1, *stmt2;
  const char *stmt1_txt= "SELECT NOW();";
  const char *stmt2_txt= "SELECT 1;";
  unsigned long thread_id;
  char query[MAX_TEST_QUERY_LENGTH];
  int rc;

  conn1= client_connect(0, MYSQL_PROTOCOL_DEFAULT, 1);
  conn2= client_connect(0, MYSQL_PROTOCOL_DEFAULT, 0);

  stmt1 = mysql_stmt_init(conn1);
  check_stmt(stmt1);
  rc= mysql_stmt_prepare(stmt1, stmt1_txt, strlen(stmt1_txt));
  check_execute(stmt1, rc);

  thread_id= mysql_thread_id(conn1);
  sprintf(query, "KILL %lu", thread_id);
  if (thread_query(query))
    exit(1);

  /*
    After the connection is killed, the connection is
    re-established due to the reconnect flag.
  */
  stmt2 = mysql_stmt_init(conn1);
  check_stmt(stmt2);

  rc= mysql_stmt_prepare(stmt2, stmt2_txt, strlen(stmt2_txt));
  check_execute(stmt1, rc);

  mysql_stmt_close(stmt2);
  mysql_stmt_close(stmt1);

  mysql_close(conn1);
  mysql_close(conn2);
}