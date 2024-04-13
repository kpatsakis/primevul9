static void test_nstmts()
{
  MYSQL_STMT  *stmt;
  char        query[255];
  int         rc;
  static uint i, total_stmts= 2000;
  MYSQL_BIND  my_bind[1];

  myheader("test_nstmts");

  mysql_autocommit(mysql, TRUE);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_nstmts");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_nstmts(id int)");
  myquery(rc);

  /*
    We need to memset bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer= (void *)&i;
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;

  for (i= 0; i < total_stmts; i++)
  {
    if (!opt_silent)
      fprintf(stdout, "\r stmt: %d", i);

    strmov(query, "insert into test_nstmts values(?)");
    stmt= mysql_simple_prepare(mysql, query);
    check_stmt(stmt);

    rc= mysql_stmt_bind_param(stmt, my_bind);
    check_execute(stmt, rc);

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    mysql_stmt_close(stmt);
  }

  stmt= mysql_simple_prepare(mysql, " select count(*) from test_nstmts");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  i= 0;
  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);
  if (!opt_silent)
    fprintf(stdout, "\n total rows: %d", i);
  DIE_UNLESS( i == total_stmts);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "DROP TABLE test_nstmts");
  myquery(rc);
}