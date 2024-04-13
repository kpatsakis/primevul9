static void test_decimal_bug()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  char       data[30];
  int        rc;
  my_bool    is_null;

  myheader("test_decimal_bug");

  mysql_autocommit(mysql, TRUE);

  rc= mysql_query(mysql, "drop table if exists test_decimal_bug");
  myquery(rc);

  rc= mysql_query(mysql, "create table test_decimal_bug(c1 decimal(10, 2))");
  myquery(rc);

  rc= mysql_query(mysql, "insert into test_decimal_bug value(8), (10.22), (5.61)");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "select c1 from test_decimal_bug where c1= ?");
  check_stmt(stmt);

  /*
    We need to memset bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_NEWDECIMAL;
  my_bind[0].buffer= (void *)data;
  my_bind[0].buffer_length= 25;
  my_bind[0].is_null= &is_null;

  is_null= 0;
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  strmov(data, "8.0");
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  data[0]= 0;
  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n data: %s", data);
  DIE_UNLESS(strcmp(data, "8.00") == 0);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  strmov(data, "5.61");
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  data[0]= 0;
  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n data: %s", data);
  DIE_UNLESS(strcmp(data, "5.61") == 0);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  is_null= 1;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  strmov(data, "10.22"); is_null= 0;
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  data[0]= 0;
  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n data: %s", data);
  DIE_UNLESS(strcmp(data, "10.22") == 0);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
}