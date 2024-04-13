static void test_sqlmode()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[2];
  char       c1[5], c2[5];
  int        rc;
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_sqlmode");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_piping");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_piping(name varchar(10))");
  myquery(rc);

  /* PIPES_AS_CONCAT */
  strmov(query, "SET SQL_MODE= \"PIPES_AS_CONCAT\"");
  if (!opt_silent)
    fprintf(stdout, "\n With %s", query);
  rc= mysql_query(mysql, query);
  myquery(rc);

  strmov(query, "INSERT INTO test_piping VALUES(?||?)");
  if (!opt_silent)
    fprintf(stdout, "\n  query: %s", query);
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  if (!opt_silent)
    fprintf(stdout, "\n  total parameters: %ld", mysql_stmt_param_count(stmt));

  /*
    We need to memset bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)c1;
  my_bind[0].buffer_length= 2;

  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= (void *)c2;
  my_bind[1].buffer_length= 3;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  strmov(c1, "My"); strmov(c2, "SQL");
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  mysql_stmt_close(stmt);

  verify_col_data("test_piping", "name", "MySQL");

  rc= mysql_query(mysql, "DELETE FROM test_piping");
  myquery(rc);

  strmov(query, "SELECT connection_id    ()");
  if (!opt_silent)
    fprintf(stdout, "\n  query: %s", query);
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);
  mysql_stmt_close(stmt);

  /* ANSI */
  strmov(query, "SET SQL_MODE= \"ANSI\"");
  if (!opt_silent)
    fprintf(stdout, "\n With %s", query);
  rc= mysql_query(mysql, query);
  myquery(rc);

  strmov(query, "INSERT INTO test_piping VALUES(?||?)");
  if (!opt_silent)
    fprintf(stdout, "\n  query: %s", query);
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);
  if (!opt_silent)
    fprintf(stdout, "\n  total parameters: %ld", mysql_stmt_param_count(stmt));

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  strmov(c1, "My"); strmov(c2, "SQL");
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  mysql_stmt_close(stmt);
  verify_col_data("test_piping", "name", "MySQL");

  /* ANSI mode spaces ... */
  strmov(query, "SELECT connection_id    ()");
  if (!opt_silent)
    fprintf(stdout, "\n  query: %s", query);
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);
  if (!opt_silent)
    fprintf(stdout, "\n  returned 1 row\n");

  mysql_stmt_close(stmt);

  /* IGNORE SPACE MODE */
  strmov(query, "SET SQL_MODE= \"IGNORE_SPACE\"");
  if (!opt_silent)
    fprintf(stdout, "\n With %s", query);
  rc= mysql_query(mysql, query);
  myquery(rc);

  strmov(query, "SELECT connection_id    ()");
  if (!opt_silent)
    fprintf(stdout, "\n  query: %s", query);
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);
  if (!opt_silent)
    fprintf(stdout, "\n  returned 1 row");

  mysql_stmt_close(stmt);
}