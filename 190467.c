static void test_prepare_noparam()
{
  MYSQL_STMT *stmt;
  int        rc;
  MYSQL_RES  *result;
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_prepare_noparam");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS my_prepare");
  myquery(rc);


  rc= mysql_query(mysql, "CREATE TABLE my_prepare(col1 int, col2 varchar(50))");
  myquery(rc);

  /* insert by prepare */
  strmov(query, "INSERT INTO my_prepare VALUES(10, 'venu')");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 0);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  mysql_stmt_close(stmt);

  /* now fetch the results ..*/
  rc= mysql_commit(mysql);
  myquery(rc);

  /* test the results now, only one row should exist */
  rc= mysql_query(mysql, "SELECT * FROM my_prepare");
  myquery(rc);

  /* get the result */
  result= mysql_store_result(mysql);
  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS(rc == 1);
  mysql_free_result(result);
}