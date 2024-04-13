static void test_select()
{
  MYSQL_STMT *stmt;
  int        rc;
  char       szData[25];
  int        nData= 1;
  MYSQL_BIND my_bind[2];
  ulong length[2];
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_select");

  rc= mysql_autocommit(mysql, TRUE);
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_select");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_select(id int, name varchar(50))");
  myquery(rc);

  /* insert a row and commit the transaction */
  rc= mysql_query(mysql, "INSERT INTO test_select VALUES(10, 'venu')");
  myquery(rc);

  /* now insert the second row, and roll back the transaction */
  rc= mysql_query(mysql, "INSERT INTO test_select VALUES(20, 'mysql')");
  myquery(rc);

  rc= mysql_commit(mysql);
  myquery(rc);

  strmov(query, "SELECT * FROM test_select WHERE id= ? "
                "AND CONVERT(name USING utf8) =?");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 2);

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  /* string data */
  nData= 10;
  strmov(szData, (char *)"venu");
  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= (void *)szData;
  my_bind[1].buffer_length= 4;
  my_bind[1].length= &length[1];
  length[1]= 4;

  my_bind[0].buffer= (void *)&nData;
  my_bind[0].buffer_type= MYSQL_TYPE_LONG;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 1);

  mysql_stmt_close(stmt);
}