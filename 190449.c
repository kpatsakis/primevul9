static void test_logs()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[2];
  char       data[255];
  ulong      length;
  int        rc;
  short      id;

  myheader("test_logs");


  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_logs");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_logs(id smallint, name varchar(20))");
  myquery(rc);

  strmov((char *)data, "INSERT INTO test_logs VALUES(?, ?)");
  stmt= mysql_simple_prepare(mysql, data);
  check_stmt(stmt);

  /*
    We need to memset bind structure because mysql_stmt_bind_param checks all
    its members.
  */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_SHORT;
  my_bind[0].buffer= (void *)&id;

  my_bind[1].buffer_type= MYSQL_TYPE_STRING;
  my_bind[1].buffer= (void *)&data;
  my_bind[1].buffer_length= 255;
  my_bind[1].length= &length;

  id= 9876;
  length= (ulong)(strmov((char *)data, "MySQL - Open Source Database")- data);

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  strmov((char *)data, "'");
  length= 1;

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  strmov((char *)data, "\"");
  length= 1;

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  length= (ulong)(strmov((char *)data, "my\'sql\'")-data);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  length= (ulong)(strmov((char *)data, "my\"sql\"")-data);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  mysql_stmt_close(stmt);

  strmov((char *)data, "INSERT INTO test_logs VALUES(20, 'mysql')");
  stmt= mysql_simple_prepare(mysql, data);
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  mysql_stmt_close(stmt);

  strmov((char *)data, "SELECT * FROM test_logs WHERE id=?");
  stmt= mysql_simple_prepare(mysql, data);
  check_stmt(stmt);

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  my_bind[1].buffer_length= 255;
  rc= mysql_stmt_bind_result(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
  {
    fprintf(stdout, "id    : %d\n", id);
    fprintf(stdout, "name  : %s(%ld)\n", data, length);
  }

  DIE_UNLESS(id == 9876);
  DIE_UNLESS(length == 19 || length == 20); /* Due to VARCHAR(20) */
  DIE_UNLESS(is_prefix(data, "MySQL - Open Source") == 1);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n name  : %s(%ld)", data, length);

  DIE_UNLESS(length == 1);
  DIE_UNLESS(strcmp(data, "'") == 0);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n name  : %s(%ld)", data, length);

  DIE_UNLESS(length == 1);
  DIE_UNLESS(strcmp(data, "\"") == 0);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n name  : %s(%ld)", data, length);

  DIE_UNLESS(length == 7);
  DIE_UNLESS(strcmp(data, "my\'sql\'") == 0);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  if (!opt_silent)
    fprintf(stdout, "\n name  : %s(%ld)", data, length);

  DIE_UNLESS(length == 7);
  /*DIE_UNLESS(strcmp(data, "my\"sql\"") == 0); */

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "DROP TABLE test_logs");
  myquery(rc);
}