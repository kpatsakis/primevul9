static void test_store_result2()
{
  MYSQL_STMT *stmt;
  int        rc;
  int        nData;
  ulong      length;
  MYSQL_BIND my_bind[1];
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_store_result2");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_store_result");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_store_result(col1 int , col2 varchar(50))");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_store_result VALUES(10, 'venu'), (20, 'mysql')");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_store_result(col2) VALUES('monty')");
  myquery(rc);

  rc= mysql_commit(mysql);
  myquery(rc);

  /*
    We need to memset bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *) &nData;      /* integer data */
  my_bind[0].length= &length;
  my_bind[0].is_null= 0;

  strmov((char *)query , "SELECT col1 FROM test_store_result where col1= ?");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  nData= 10; length= 0;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  nData= 0;
  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 1: %d", nData);
  DIE_UNLESS(nData == 10);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  nData= 20;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  nData= 0;
  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n row 1: %d", nData);
  DIE_UNLESS(nData == 20);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);
  mysql_stmt_close(stmt);
}