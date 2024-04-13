static void test_update()
{
  MYSQL_STMT *stmt;
  int        rc;
  char       szData[25];
  int        nData= 1;
  MYSQL_RES  *result;
  MYSQL_BIND my_bind[2];
  ulong length[2];
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_update");

  rc= mysql_autocommit(mysql, TRUE);
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_update");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_update("
                               "col1 int primary key auto_increment, "
                               "col2 varchar(50), col3 int )");
  myquery(rc);

  strmov(query, "INSERT INTO test_update(col2, col3) VALUES(?, ?)");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 2);

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  /* string data */
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= szData;
  my_bind[0].buffer_length= sizeof(szData);
  my_bind[0].length= &length[0];
  length[0]= sprintf(szData, "inserted-data");

  my_bind[1].buffer= (void *)&nData;
  my_bind[1].buffer_type= MYSQL_TYPE_LONG;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  nData= 100;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  verify_affected_rows(1);
  mysql_stmt_close(stmt);

  strmov(query, "UPDATE test_update SET col2= ? WHERE col3= ?");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 2);
  nData= 100;

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= szData;
  my_bind[0].buffer_length= sizeof(szData);
  my_bind[0].length= &length[0];
  length[0]= sprintf(szData, "updated-data");

  my_bind[1].buffer= (void *)&nData;
  my_bind[1].buffer_type= MYSQL_TYPE_LONG;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  verify_affected_rows(1);

  mysql_stmt_close(stmt);

  /* now fetch the results ..*/
  rc= mysql_commit(mysql);
  myquery(rc);

  /* test the results now, only one row should exist */
  rc= mysql_query(mysql, "SELECT * FROM test_update");
  myquery(rc);

  /* get the result */
  result= mysql_store_result(mysql);
  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS(rc == 1);
  mysql_free_result(result);
}