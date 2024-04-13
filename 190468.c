static void test_bug36004()
{
  int rc, warning_count= 0;
  MYSQL_STMT *stmt;

  DBUG_ENTER("test_bug36004");
  myheader("test_bug36004");

  rc= mysql_query(mysql, "drop table if exists inexistant");
  myquery(rc);

  DIE_UNLESS(mysql_warning_count(mysql) == 1);
  query_int_variable(mysql, "@@warning_count", &warning_count);
  DIE_UNLESS(warning_count);

  stmt= mysql_simple_prepare(mysql, "select 1");
  check_stmt(stmt);

  DIE_UNLESS(mysql_warning_count(mysql) == 0);
  query_int_variable(mysql, "@@warning_count", &warning_count);
  DIE_UNLESS(warning_count);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  DIE_UNLESS(mysql_warning_count(mysql) == 0);
  mysql_stmt_close(stmt);

  query_int_variable(mysql, "@@warning_count", &warning_count);
  DIE_UNLESS(warning_count);

  stmt= mysql_simple_prepare(mysql, "drop table if exists inexistant");
  check_stmt(stmt);

  query_int_variable(mysql, "@@warning_count", &warning_count);
  DIE_UNLESS(warning_count == 0);
  mysql_stmt_close(stmt);

  DBUG_VOID_RETURN;
}