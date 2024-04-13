static void test_bug1180()
{
  MYSQL_STMT *stmt;
  int rc;
  MYSQL_BIND my_bind[1];
  ulong length[1];
  char szData[11];
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_select_bug");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_select");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_select(session_id  char(9) NOT NULL)");
  myquery(rc);
  rc= mysql_query(mysql, "INSERT INTO test_select VALUES (\"abc\")");
  myquery(rc);

  strmov(query, "SELECT * FROM test_select WHERE ?= \"1111\" and "
                "session_id= \"abc\"");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 1);

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  strmov(szData, (char *)"abc");
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)szData;
  my_bind[0].buffer_length= 10;
  my_bind[0].length= &length[0];
  length[0]= 3;
  my_bind[0].is_null= 0;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 0);

  strmov(szData, (char *)"1111");
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)szData;
  my_bind[0].buffer_length= 10;
  my_bind[0].length= &length[0];
  length[0]= 4;
  my_bind[0].is_null= 0;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 1);

  strmov(szData, (char *)"abc");
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)szData;
  my_bind[0].buffer_length= 10;
  my_bind[0].length= &length[0];
  length[0]= 3;
  my_bind[0].is_null= 0;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 0);

  mysql_stmt_close(stmt);
}