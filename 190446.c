static void test_prepare_field_result()
{
  MYSQL_STMT *stmt;
  MYSQL_RES  *result;
  int        rc;
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_prepare_field_result");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_prepare_field_result");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_prepare_field_result(int_c int, "
                         "var_c varchar(50), ts_c timestamp, "
                         "char_c char(4), date_c date, extra tinyint)");
  myquery(rc);

  /* insert */
  strmov(query, "SELECT int_c, var_c, date_c as date, ts_c, char_c FROM "
                " test_prepare_field_result as t1 WHERE int_c=?");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 1);

  result= mysql_stmt_result_metadata(stmt);
  mytest(result);

  my_print_result_metadata(result);

  if (!opt_silent)
    fprintf(stdout, "\n\n field attributes:\n");
  verify_prepare_field(result, 0, "int_c", "int_c", MYSQL_TYPE_LONG,
                       "t1", "test_prepare_field_result", current_db, 11, 0);
  verify_prepare_field(result, 1, "var_c", "var_c", MYSQL_TYPE_VAR_STRING,
                       "t1", "test_prepare_field_result", current_db, 50, 0);
  verify_prepare_field(result, 2, "date", "date_c", MYSQL_TYPE_DATE,
                       "t1", "test_prepare_field_result", current_db, 10, 0);
  verify_prepare_field(result, 3, "ts_c", "ts_c", MYSQL_TYPE_TIMESTAMP,
                       "t1", "test_prepare_field_result", current_db, 19, 0);
  verify_prepare_field(result, 4, "char_c", "char_c",
                       (mysql_get_server_version(mysql) <= 50000 ?
                        MYSQL_TYPE_VAR_STRING : MYSQL_TYPE_STRING),
                       "t1", "test_prepare_field_result", current_db, 4, 0);

  verify_field_count(result, 5);
  mysql_free_result(result);
  mysql_stmt_close(stmt);
}