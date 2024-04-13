static void test_long_data()
{
  MYSQL_STMT *stmt;
  int        rc, int_data;
  char       *data= NullS;
  MYSQL_RES  *result;
  MYSQL_BIND my_bind[3];
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_long_data");

  rc= mysql_autocommit(mysql, TRUE);
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_long_data");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_long_data(col1 int, "
                         "      col2 long varchar, col3 long varbinary)");
  myquery(rc);

  strmov(query, "INSERT INTO test_long_data(col1, col2) VALUES(?)");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt_r(stmt);

  strmov(query, "INSERT INTO test_long_data(col1, col2, col3) VALUES(?, ?, ?)");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 3);

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer= (void *)&int_data;
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;

  my_bind[1].buffer_type= MYSQL_TYPE_STRING;

  my_bind[2]= my_bind[1];
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  int_data= 999;
  data= (char *)"Michael";

  /* supply data in pieces */
  rc= mysql_stmt_send_long_data(stmt, 1, data, strlen(data));
  data= (char *)" 'Monty' Widenius";
  rc= mysql_stmt_send_long_data(stmt, 1, data, strlen(data));
  check_execute(stmt, rc);
  rc= mysql_stmt_send_long_data(stmt, 2, "Venu (venu@mysql.com)", 4);
  check_execute(stmt, rc);

  /* execute */
  rc= mysql_stmt_execute(stmt);
  if (!opt_silent)
    fprintf(stdout, " mysql_stmt_execute() returned %d\n", rc);
  check_execute(stmt, rc);

  rc= mysql_commit(mysql);
  myquery(rc);

  /* now fetch the results ..*/
  rc= mysql_query(mysql, "SELECT * FROM test_long_data");
  myquery(rc);

  /* get the result */
  result= mysql_store_result(mysql);
  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS(rc == 1);
  mysql_free_result(result);

  verify_col_data("test_long_data", "col1", "999");
  verify_col_data("test_long_data", "col2", "Michael 'Monty' Widenius");
  verify_col_data("test_long_data", "col3", "Venu");
  mysql_stmt_close(stmt);
}