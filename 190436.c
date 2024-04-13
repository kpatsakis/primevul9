static void test_prepare_multi_statements()
{
  MYSQL *mysql_local;
  MYSQL_STMT *stmt;
  char query[MAX_TEST_QUERY_LENGTH];
  myheader("test_prepare_multi_statements");

  if (!(mysql_local= mysql_client_init(NULL)))
  {
    fprintf(stderr, "\n mysql_client_init() failed");
    exit(1);
  }

  if (!(mysql_real_connect(mysql_local, opt_host, opt_user,
                           opt_password, current_db, opt_port,
                           opt_unix_socket, CLIENT_MULTI_STATEMENTS)))
  {
    fprintf(stderr, "\n connection failed(%s)", mysql_error(mysql_local));
    exit(1);
  }
  mysql_local->reconnect= 1;
  strmov(query, "select 1; select 'another value'");
  stmt= mysql_simple_prepare(mysql_local, query);
  check_stmt_r(stmt);
  mysql_close(mysql_local);
}