static void test_bug17309863()
{
  MYSQL *lmysql;
  unsigned long thread_id;
  char query[MAX_TEST_QUERY_LENGTH];
  int rc;

  myheader("test_bug17309863");

  if (!opt_silent)
    fprintf(stdout, "\n Establishing a test connection ...");
  if (!(lmysql= mysql_client_init(NULL)))
  {
    myerror("mysql_client_init() failed");
    exit(1);
  }
  lmysql->reconnect= 1;
  if (!(mysql_real_connect(lmysql, opt_host, opt_user,
                           opt_password, current_db, opt_port,
                           opt_unix_socket, 0)))
  {
    myerror("connection failed");
    exit(1);
  }
  if (!opt_silent)
    fprintf(stdout, "OK");

  thread_id= mysql_thread_id(lmysql);
  sprintf(query, "KILL %lu", thread_id);

  /*
    Running the "KILL <thread_id>" query in a separate connection.
  */
  if (thread_query(query))
    exit(1);

  /*
    The above KILL statement should have closed our connection. But reconnect
    flag allows to detect this before sending query and re-establish it without
    returning an error.
  */
  rc= mysql_query(lmysql, "SELECT 'bug17309863'");
  myquery(rc);

  mysql_close(lmysql);
}