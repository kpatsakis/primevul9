static void test_prepare_simple()
{
  MYSQL_STMT *stmt;
  int        rc;
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_prepare_simple");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_prepare_simple");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_prepare_simple("
                         "id int, name varchar(50))");
  myquery(rc);

  /* insert */
  strmov(query, "INSERT INTO test_prepare_simple VALUES(?, ?)");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 2);
  mysql_stmt_close(stmt);

  /* update */
  strmov(query, "UPDATE test_prepare_simple SET id=? "
                "WHERE id=? AND CONVERT(name USING utf8)= ?");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 3);
  mysql_stmt_close(stmt);

  /* delete */
  strmov(query, "DELETE FROM test_prepare_simple WHERE id=10");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 0);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  mysql_stmt_close(stmt);

  /* delete */
  strmov(query, "DELETE FROM test_prepare_simple WHERE id=?");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 1);

  mysql_stmt_close(stmt);

  /* select */
  strmov(query, "SELECT * FROM test_prepare_simple WHERE id=? "
                "AND CONVERT(name USING utf8)= ?");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 2);

  mysql_stmt_close(stmt);

  /* now fetch the results ..*/
  rc= mysql_commit(mysql);
  myquery(rc);
}