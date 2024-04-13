static void test_explain_bug()
{
  MYSQL_STMT *stmt;
  MYSQL_RES  *result;
  int        rc;

  myheader("test_explain_bug");

  mysql_autocommit(mysql, TRUE);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_explain");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_explain(id int, name char(2))");
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "explain test_explain");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 2);

  result= mysql_stmt_result_metadata(stmt);
  mytest(result);

  if (!opt_silent)
    fprintf(stdout, "\n total fields in the result: %d",
            mysql_num_fields(result));
  DIE_UNLESS(6 == mysql_num_fields(result));

  verify_prepare_field(result, 0, "Field", "COLUMN_NAME",
                       mysql_get_server_version(mysql) <= 50000 ?
                       MYSQL_TYPE_STRING : MYSQL_TYPE_VAR_STRING,
                       0, 0, "information_schema", 64, 0);

  verify_prepare_field(result, 1, "Type", "COLUMN_TYPE", MYSQL_TYPE_BLOB,
                       0, 0, "information_schema", 0, 0);

  verify_prepare_field(result, 2, "Null", "IS_NULLABLE",
                       mysql_get_server_version(mysql) <= 50000 ?
                       MYSQL_TYPE_STRING : MYSQL_TYPE_VAR_STRING,
                       0, 0, "information_schema", 3, 0);

  verify_prepare_field(result, 3, "Key", "COLUMN_KEY",
                       mysql_get_server_version(mysql) <= 50000 ?
                       MYSQL_TYPE_STRING : MYSQL_TYPE_VAR_STRING,
                       0, 0, "information_schema", 3, 0);

  if ( mysql_get_server_version(mysql) >= 50027 )
  {
    /*  The patch for bug#23037 changes column type of DEAULT to blob */
    verify_prepare_field(result, 4, "Default", "COLUMN_DEFAULT",
                         MYSQL_TYPE_BLOB, 0, 0, "information_schema", 0, 0);
  }
  else
  {
    verify_prepare_field(result, 4, "Default", "COLUMN_DEFAULT",
                         mysql_get_server_version(mysql) >= 50027 ?
                         MYSQL_TYPE_BLOB :
                         mysql_get_server_version(mysql) <= 50000 ?
                         MYSQL_TYPE_STRING : MYSQL_TYPE_VAR_STRING,
                         0, 0, "information_schema",
                         mysql_get_server_version(mysql) >= 50027 ? 0 :64, 0);
  }

  verify_prepare_field(result, 5, "Extra", "EXTRA",
                       mysql_get_server_version(mysql) <= 50000 ?
                       MYSQL_TYPE_STRING : MYSQL_TYPE_VAR_STRING,
                       0, 0, "information_schema",
                       mysql_get_server_version(mysql) <= 50602 ? 27 : 30,
                       0);

  mysql_free_result(result);
  mysql_stmt_close(stmt);

  stmt= mysql_simple_prepare(mysql, "explain select id, name FROM test_explain");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 1);

  result= mysql_stmt_result_metadata(stmt);
  mytest(result);

  if (!opt_silent)
    fprintf(stdout, "\n total fields in the result: %d",
            mysql_num_fields(result));
  DIE_UNLESS(10 == mysql_num_fields(result));

  verify_prepare_field(result, 0, "id", "", MYSQL_TYPE_LONGLONG,
                       "", "", "", 3, 0);

  verify_prepare_field(result, 1, "select_type", "", MYSQL_TYPE_VAR_STRING,
                       "", "", "", 19, 0);

  verify_prepare_field(result, 2, "table", "", MYSQL_TYPE_VAR_STRING,
                       "", "", "", NAME_CHAR_LEN, 0);

  verify_prepare_field(result, 3, "type", "", MYSQL_TYPE_VAR_STRING,
                       "", "", "", 10, 0);

  verify_prepare_field(result, 4, "possible_keys", "", MYSQL_TYPE_VAR_STRING,
                       "", "", "", NAME_CHAR_LEN*MAX_KEY, 0);

  verify_prepare_field(result, 5, "key", "", MYSQL_TYPE_VAR_STRING,
                       "", "", "", NAME_CHAR_LEN, 0);

  if (mysql_get_server_version(mysql) <= 50000)
  {
    verify_prepare_field(result, 6, "key_len", "", MYSQL_TYPE_LONGLONG, "",
                         "", "", 3, 0);
  }
  else
  {
    verify_prepare_field(result, 6, "key_len", "", MYSQL_TYPE_VAR_STRING, "", 
                         "", "", NAME_CHAR_LEN*MAX_KEY, 0);
  }

  verify_prepare_field(result, 7, "ref", "", MYSQL_TYPE_VAR_STRING,
                       "", "", "", NAME_CHAR_LEN*16, 0);

  verify_prepare_field(result, 8, "rows", "", MYSQL_TYPE_LONGLONG,
                       "", "", "", 10, 0);

  verify_prepare_field(result, 9, "Extra", "", MYSQL_TYPE_VAR_STRING,
                       "", "", "", 255, 0);

  mysql_free_result(result);
  mysql_stmt_close(stmt);
}