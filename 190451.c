static void test_bug1644()
{
  MYSQL_STMT *stmt;
  MYSQL_RES *result;
  MYSQL_ROW row;
  MYSQL_BIND my_bind[4];
  int num;
  my_bool isnull;
  int rc, i;
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_bug1644");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS foo_dfr");
  myquery(rc);

  rc= mysql_query(mysql,
           "CREATE TABLE foo_dfr(col1 int, col2 int, col3 int, col4 int);");
  myquery(rc);

  strmov(query, "INSERT INTO foo_dfr VALUES (?, ?, ?, ? )");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 4);

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  num= 22;
  isnull= 0;
  for (i= 0 ; i < 4 ; i++)
  {
    my_bind[i].buffer_type= MYSQL_TYPE_LONG;
    my_bind[i].buffer= (void *)&num;
    my_bind[i].is_null= &isnull;
  }

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  isnull= 1;
  for (i= 0 ; i < 4 ; i++)
    my_bind[i].is_null= &isnull;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  isnull= 0;
  num= 88;
  for (i= 0 ; i < 4 ; i++)
    my_bind[i].is_null= &isnull;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "SELECT * FROM foo_dfr");
  myquery(rc);

  result= mysql_store_result(mysql);
  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS(rc == 3);

  mysql_data_seek(result, 0);

  row= mysql_fetch_row(result);
  mytest(row);
  for (i= 0 ; i < 4 ; i++)
  {
    DIE_UNLESS(strcmp(row[i], "22") == 0);
  }
  row= mysql_fetch_row(result);
  mytest(row);
  for (i= 0 ; i < 4 ; i++)
  {
    DIE_UNLESS(row[i] == 0);
  }
  row= mysql_fetch_row(result);
  mytest(row);
  for (i= 0 ; i < 4 ; i++)
  {
    DIE_UNLESS(strcmp(row[i], "88") == 0);
  }
  row= mysql_fetch_row(result);
  mytest_r(row);

  mysql_free_result(result);
}