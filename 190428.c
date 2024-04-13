static void test_union_param()
{
  MYSQL_STMT *stmt;
  char *query;
  int rc, i;
  MYSQL_BIND      my_bind[2];
  char            my_val[4];
  ulong           my_length= 3L;
  my_bool         my_null= FALSE;
  myheader("test_union_param");

  strmov(my_val, "abc");

  query= (char*)"select ? as my_col union distinct select ?";
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  /*
    We need to memset bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  memset(my_bind, 0, sizeof(my_bind));

  /* bind parameters */
  my_bind[0].buffer_type=    MYSQL_TYPE_STRING;
  my_bind[0].buffer=         (char*) &my_val;
  my_bind[0].buffer_length=  4;
  my_bind[0].length=         &my_length;
  my_bind[0].is_null=        (char*)&my_null;
  my_bind[1].buffer_type=    MYSQL_TYPE_STRING;
  my_bind[1].buffer=         (char*) &my_val;
  my_bind[1].buffer_length=  4;
  my_bind[1].length=         &my_length;
  my_bind[1].is_null=        (char*)&my_null;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  for (i= 0; i < 3; i++)
  {
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    rc= my_process_stmt_result(stmt);
    DIE_UNLESS(rc == 1);
  }

  mysql_stmt_close(stmt);
}