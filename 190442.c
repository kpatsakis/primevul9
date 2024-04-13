static void test_stmt_close()
{
  MYSQL *lmysql;
  MYSQL_STMT *stmt1, *stmt2, *stmt3, *stmt_x;
  MYSQL_BIND  my_bind[1];
  MYSQL_RES   *result;
  unsigned int  count;
  int   rc;
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_stmt_close");

  if (!opt_silent)
    fprintf(stdout, "\n Establishing a test connection ...");
  if (!(lmysql= mysql_client_init(NULL)))
  {
    myerror("mysql_client_init() failed");
    exit(1);
  }
  if (!(mysql_real_connect(lmysql, opt_host, opt_user,
                           opt_password, current_db, opt_port,
                           opt_unix_socket, 0)))
  {
    myerror("connection failed");
    exit(1);
  }
  lmysql->reconnect= 1;
  if (!opt_silent)
    fprintf(stdout, "OK");


  /* set AUTOCOMMIT to ON*/
  mysql_autocommit(lmysql, TRUE);

  rc= mysql_query(lmysql, "SET SQL_MODE = ''");
  myquery(rc);

  rc= mysql_query(lmysql, "DROP TABLE IF EXISTS test_stmt_close");
  myquery(rc);

  rc= mysql_query(lmysql, "CREATE TABLE test_stmt_close(id int)");
  myquery(rc);

  strmov(query, "DO \"nothing\"");
  stmt1= mysql_simple_prepare(lmysql, query);
  check_stmt(stmt1);

  verify_param_count(stmt1, 0);

  strmov(query, "INSERT INTO test_stmt_close(id) VALUES(?)");
  stmt_x= mysql_simple_prepare(mysql, query);
  check_stmt(stmt_x);

  verify_param_count(stmt_x, 1);

  strmov(query, "UPDATE test_stmt_close SET id= ? WHERE id= ?");
  stmt3= mysql_simple_prepare(lmysql, query);
  check_stmt(stmt3);

  verify_param_count(stmt3, 2);

  strmov(query, "SELECT * FROM test_stmt_close WHERE id= ?");
  stmt2= mysql_simple_prepare(lmysql, query);
  check_stmt(stmt2);

  verify_param_count(stmt2, 1);

  rc= mysql_stmt_close(stmt1);
  if (!opt_silent)
    fprintf(stdout, "\n mysql_close_stmt(1) returned: %d", rc);
  DIE_UNLESS(rc == 0);

  /*
    Originally we were going to close all statements automatically in
    mysql_close(). This proved to not work well - users weren't able to
    close statements by hand once mysql_close() had been called.
    Now mysql_close() doesn't free any statements, so this test doesn't
    serve its original designation any more.
    Here we free stmt2 and stmt3 by hand to avoid memory leaks.
  */
  mysql_stmt_close(stmt2);
  mysql_stmt_close(stmt3);
  mysql_close(lmysql);

  /*
    We need to memset bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer= (void *)&count;
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  count= 100;

  rc= mysql_stmt_bind_param(stmt_x, my_bind);
  check_execute(stmt_x, rc);

  rc= mysql_stmt_execute(stmt_x);
  check_execute(stmt_x, rc);

  verify_st_affected_rows(stmt_x, 1);

  rc= mysql_stmt_close(stmt_x);
  if (!opt_silent)
    fprintf(stdout, "\n mysql_close_stmt(x) returned: %d", rc);
  DIE_UNLESS( rc == 0);

  rc= mysql_query(mysql, "SELECT id FROM test_stmt_close");
  myquery(rc);

  result= mysql_store_result(mysql);
  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS(rc == 1);
  mysql_free_result(result);
}