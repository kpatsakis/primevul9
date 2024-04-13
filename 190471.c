static void test_null()
{
  MYSQL_STMT *stmt;
  int        rc;
  uint       nData;
  MYSQL_BIND my_bind[2];
  my_bool    is_null[2];
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_null");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_null");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_null(col1 int, col2 varchar(50))");
  myquery(rc);

  /* insert by prepare, wrong column name */
  strmov(query, "INSERT INTO test_null(col3, col2) VALUES(?, ?)");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt_r(stmt);

  strmov(query, "INSERT INTO test_null(col1, col2) VALUES(?, ?)");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 2);

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].is_null= &is_null[0];
  is_null[0]= 1;
  my_bind[1]= my_bind[0];

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  /* now, execute the prepared statement to insert 10 records.. */
  for (nData= 0; nData<10; nData++)
  {
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
  }

  /* Re-bind with MYSQL_TYPE_NULL */
  my_bind[0].buffer_type= MYSQL_TYPE_NULL;
  is_null[0]= 0; /* reset */
  my_bind[1]= my_bind[0];

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  for (nData= 0; nData<10; nData++)
  {
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
  }

  mysql_stmt_close(stmt);

  /* now fetch the results ..*/
  rc= mysql_commit(mysql);
  myquery(rc);

  nData*= 2;
  rc= my_stmt_result("SELECT * FROM test_null");;
  DIE_UNLESS((int) nData == rc);

  /* Fetch results */
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&nData; /* this buffer won't be altered */
  my_bind[0].length= 0;
  my_bind[1]= my_bind[0];
  my_bind[0].is_null= &is_null[0];
  my_bind[1].is_null= &is_null[1];

  stmt= mysql_simple_prepare(mysql, "SELECT * FROM test_null");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= 0;
  is_null[0]= is_null[1]= 0;
  while (mysql_stmt_fetch(stmt) != MYSQL_NO_DATA)
  {
    DIE_UNLESS(is_null[0]);
    DIE_UNLESS(is_null[1]);
    rc++;
    is_null[0]= is_null[1]= 0;
  }
  DIE_UNLESS(rc == (int) nData);
  mysql_stmt_close(stmt);
}