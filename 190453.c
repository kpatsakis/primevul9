static void test_prepare_syntax()
{
  MYSQL_STMT *stmt;
  int        rc;
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_prepare_syntax");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_prepare_syntax");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_prepare_syntax("
                         "id int, name varchar(50), extra int)");
  myquery(rc);

  strmov(query, "INSERT INTO test_prepare_syntax VALUES(?");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt_r(stmt);

  strmov(query, "SELECT id, name FROM test_prepare_syntax WHERE id=? AND WHERE");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt_r(stmt);

  /* now fetch the results ..*/
  rc= mysql_commit(mysql);
  myquery(rc);
}