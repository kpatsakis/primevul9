static void test_double_compare()
{
  MYSQL_STMT *stmt;
  int        rc;
  char       real_data[10], tiny_data;
  double     double_data;
  MYSQL_RES  *result;
  MYSQL_BIND my_bind[3];
  ulong      length[3];
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_double_compare");

  rc= mysql_autocommit(mysql, TRUE);
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_double_compare");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_double_compare(col1 tinyint, "
                         " col2 float, col3 double )");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_double_compare "
                         "VALUES (1, 10.2, 34.5)");
  myquery(rc);

  strmov(query, "UPDATE test_double_compare SET col1=100 "
                "WHERE col1 = ? AND col2 = ? AND COL3 = ?");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 3);

  /* Always memset bind array because there can be internal members */
  memset(my_bind, 0, sizeof(my_bind));

  /* tinyint */
  my_bind[0].buffer_type= MYSQL_TYPE_TINY;
  my_bind[0].buffer= (void *)&tiny_data;

  /* string->float */
  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= (void *)&real_data;
  my_bind[1].buffer_length= sizeof(real_data);
  my_bind[1].length= &length[1];
  length[1]= 10;

  /* double */
  my_bind[2].buffer_type= MYSQL_TYPE_DOUBLE;
  my_bind[2].buffer= (void *)&double_data;

  tiny_data= 1;
  strmov(real_data, "10.2");
  double_data= 34.5;
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  verify_affected_rows(0);

  mysql_stmt_close(stmt);

  /* now fetch the results ..*/
  rc= mysql_commit(mysql);
  myquery(rc);

  /* test the results now, only one row should exist */
  rc= mysql_query(mysql, "SELECT * FROM test_double_compare");
  myquery(rc);

  /* get the result */
  result= mysql_store_result(mysql);
  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS((int)tiny_data == rc);
  mysql_free_result(result);
}