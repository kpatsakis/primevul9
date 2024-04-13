static void test_ts()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[6];
  MYSQL_TIME ts;
  MYSQL_RES  *prep_res;
  char       strts[30];
  ulong      length;
  int        rc, field_count;
  char       name;
  char query[MAX_TEST_QUERY_LENGTH];
  const char *queries [3]= {"SELECT a, b, c FROM test_ts WHERE %c=?",
                            "SELECT a, b, c FROM test_ts WHERE %c=?",
                            "SELECT a, b, c FROM test_ts WHERE %c=CAST(? AS DATE)"};
  myheader("test_ts");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_ts");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_ts(a DATE, b TIME, c TIMESTAMP)");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "INSERT INTO test_ts VALUES(?, ?, ?), (?, ?, ?)");
  check_stmt(stmt);

  ts.year= 2003;
  ts.month= 07;
  ts.day= 12;
  ts.hour= 21;
  ts.minute= 07;
  ts.second= 46;
  ts.second_part= 0;
  length= (long)(strmov(strts, "2003-07-12 21:07:46") - strts);

  /*
    We need to memset bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_TIMESTAMP;
  my_bind[0].buffer= (void *)&ts;
  my_bind[0].buffer_length= sizeof(ts);

  my_bind[2]= my_bind[1]= my_bind[0];

  my_bind[3].buffer_type= MYSQL_TYPE_STRING;
  my_bind[3].buffer= (void *)strts;
  my_bind[3].buffer_length= sizeof(strts);
  my_bind[3].length= &length;

  my_bind[5]= my_bind[4]= my_bind[3];

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  mysql_stmt_close(stmt);

  verify_col_data("test_ts", "a", "2003-07-12");
  verify_col_data("test_ts", "b", "21:07:46");
  verify_col_data("test_ts", "c", "2003-07-12 21:07:46");

  stmt= mysql_simple_prepare(mysql, "SELECT * FROM test_ts");
  check_stmt(stmt);

  prep_res= mysql_stmt_result_metadata(stmt);
  mytest(prep_res);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 2);
  field_count= mysql_num_fields(prep_res);

  mysql_free_result(prep_res);
  mysql_stmt_close(stmt);

  for (name= 'a'; field_count--; name++)
  {
    int row_count= 0;

    sprintf(query, queries[field_count], name);

    if (!opt_silent)
      fprintf(stdout, "\n  %s", query);
    stmt= mysql_simple_prepare(mysql, query);
    check_stmt(stmt);

    rc= mysql_stmt_bind_param(stmt, my_bind);
    check_execute(stmt, rc);

    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);

    while (mysql_stmt_fetch(stmt) == 0)
      row_count++;

    if (!opt_silent)
      fprintf(stdout, "\n   returned '%d' rows", row_count);
    DIE_UNLESS(row_count == 2);
    mysql_stmt_close(stmt);
  }
}