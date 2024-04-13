static void test_prepare_insert_update()
{
  MYSQL_STMT *stmt;
  int        rc;
  int        i;
  const char *testcase[]= {
    "CREATE TABLE t1 (a INT, b INT, c INT, UNIQUE (A), UNIQUE(B))",
    "INSERT t1 VALUES (1,2,10), (3,4,20)",
    "INSERT t1 VALUES (5,6,30), (7,4,40), (8,9,60) ON DUPLICATE KEY UPDATE c=c+100",
    "SELECT * FROM t1",
    "INSERT t1 SET a=5 ON DUPLICATE KEY UPDATE b=0",
    "SELECT * FROM t1",
    "INSERT t1 VALUES (2,1,11), (7,4,40) ON DUPLICATE KEY UPDATE c=c+VALUES(a)",
    NULL};
  const char **cur_query;

  myheader("test_prepare_insert_update");
  
  for (cur_query= testcase; *cur_query; cur_query++)
  {
    char query[MAX_TEST_QUERY_LENGTH];
    printf("\nRunning query: %s", *cur_query);
    strmov(query, *cur_query);
    stmt= mysql_simple_prepare(mysql, query);
    check_stmt(stmt);

    verify_param_count(stmt, 0);
    rc= mysql_stmt_execute(stmt);

    check_execute(stmt, rc);
    /* try the last query several times */
    if (!cur_query[1])
    {
      for (i=0; i < 3;i++)
      {
        printf("\nExecuting last statement again");
        rc= mysql_stmt_execute(stmt);
        check_execute(stmt, rc);
        rc= mysql_stmt_execute(stmt);
        check_execute(stmt, rc);
      }
    }
    mysql_stmt_close(stmt);
  }

  rc= mysql_commit(mysql);
  myquery(rc);
}