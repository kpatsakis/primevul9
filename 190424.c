static void test_bug6049()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  MYSQL_RES *res;
  MYSQL_ROW row;
  const char *stmt_text;
  char buffer[30];
  ulong length;
  int rc;

  myheader("test_bug6049");

  stmt_text= "SELECT MAKETIME(-25, 12, 12)";

  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
  res= mysql_store_result(mysql);
  row= mysql_fetch_row(res);

  stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  memset(my_bind, 0, sizeof(my_bind));
  my_bind[0].buffer_type    = MYSQL_TYPE_STRING;
  my_bind[0].buffer         = &buffer;
  my_bind[0].buffer_length  = sizeof(buffer);
  my_bind[0].length         = &length;

  mysql_stmt_bind_result(stmt, my_bind);
  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == 0);

  if (!opt_silent)
  {
    printf("Result from query: %s\n", row[0]);
    printf("Result from prepared statement: %s\n", (char*) buffer);
  }

  DIE_UNLESS(strcmp(row[0], (char*) buffer) == 0);

  mysql_free_result(res);
  mysql_stmt_close(stmt);
}