static void test_simple_delete()
{
  MYSQL_STMT *stmt;
  int        rc;
  char       szData[30]= {0};
  int        nData= 1;
  MYSQL_RES  *result;
  MYSQL_BIND my_bind[2];
  ulong length[2];
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_simple_delete");

  rc= mysql_autocommit(mysql, TRUE);
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_simple_delete");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_simple_delete(col1 int, \
                                col2 varchar(50), col3 int )");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_simple_delete VALUES(1, 'MySQL', 100)");
  myquery(rc);

  verify_affected_rows(1);

  rc= mysql_commit(mysql);
  myquery(rc);

  /* insert by prepare */
  strmov(query, "DELETE FROM test_simple_delete WHERE col1= ? AND "
                "CONVERT(col2 USING utf8)= ? AND col3= 100");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 2);

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  nData= 1;
  strmov(szData, "MySQL");
  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= szData;               /* string data */
  my_bind[1].buffer_length= sizeof(szData);
  my_bind[1].length= &length[1];
  length[1]= 5;

  my_bind[0].buffer= (void *)&nData;
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;

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
  rc= mysql_query(mysql, "SELECT * FROM test_simple_delete");
  myquery(rc);

  /* get the result */
  result= mysql_store_result(mysql);
  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS(rc == 0);
  mysql_free_result(result);
}