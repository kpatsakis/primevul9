static void test_bug1115()
{
  MYSQL_STMT *stmt;
  int rc;
  MYSQL_BIND my_bind[1];
  ulong length[1];
  char szData[11];
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_bug1115");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_select");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_select(\
session_id  char(9) NOT NULL, \
    a       int(8) unsigned NOT NULL, \
    b        int(5) NOT NULL, \
    c      int(5) NOT NULL, \
    d  datetime NOT NULL)");
  myquery(rc);
  rc= mysql_query(mysql, "INSERT INTO test_select VALUES "
                         "(\"abc\", 1, 2, 3, 2003-08-30), "
                         "(\"abd\", 1, 2, 3, 2003-08-30), "
                         "(\"abf\", 1, 2, 3, 2003-08-30), "
                         "(\"abg\", 1, 2, 3, 2003-08-30), "
                         "(\"abh\", 1, 2, 3, 2003-08-30), "
                         "(\"abj\", 1, 2, 3, 2003-08-30), "
                         "(\"abk\", 1, 2, 3, 2003-08-30), "
                         "(\"abl\", 1, 2, 3, 2003-08-30), "
                         "(\"abq\", 1, 2, 3, 2003-08-30) ");
  myquery(rc);
  rc= mysql_query(mysql, "INSERT INTO test_select VALUES "
                         "(\"abw\", 1, 2, 3, 2003-08-30), "
                         "(\"abe\", 1, 2, 3, 2003-08-30), "
                         "(\"abr\", 1, 2, 3, 2003-08-30), "
                         "(\"abt\", 1, 2, 3, 2003-08-30), "
                         "(\"aby\", 1, 2, 3, 2003-08-30), "
                         "(\"abu\", 1, 2, 3, 2003-08-30), "
                         "(\"abi\", 1, 2, 3, 2003-08-30), "
                         "(\"abo\", 1, 2, 3, 2003-08-30), "
                         "(\"abp\", 1, 2, 3, 2003-08-30), "
                         "(\"abz\", 1, 2, 3, 2003-08-30), "
                         "(\"abx\", 1, 2, 3, 2003-08-30)");
  myquery(rc);

  strmov(query, "SELECT * FROM test_select WHERE "
                "CONVERT(session_id USING utf8)= ?");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 1);

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  strmov(szData, (char *)"abc");
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)szData;
  my_bind[0].buffer_length= 10;
  my_bind[0].length= &length[0];
  length[0]= 3;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 1);

  strmov(szData, (char *)"venu");
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)szData;
  my_bind[0].buffer_length= 10;
  my_bind[0].length= &length[0];
  length[0]= 4;
  my_bind[0].is_null= 0;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 0);

  strmov(szData, (char *)"abc");
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *)szData;
  my_bind[0].buffer_length= 10;
  my_bind[0].length= &length[0];
  length[0]= 3;
  my_bind[0].is_null= 0;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 1);

  mysql_stmt_close(stmt);
}