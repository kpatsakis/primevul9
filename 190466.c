static void test_ps_null_param()
{
  MYSQL_STMT *stmt;
  int        rc;

  MYSQL_BIND in_bind;
  my_bool    in_is_null;
  long int   in_long;

  MYSQL_BIND out_bind;
  ulong      out_length;
  my_bool    out_is_null;
  char       out_str_data[20];

  const char *queries[]= {"select ?", "select ?+1",
                    "select col1 from test_ps_nulls where col1 <=> ?",
                    NULL
                    };
  const char **cur_query= queries;

  myheader("test_null_ps_param_in_result");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_ps_nulls");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_ps_nulls(col1 int)");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO test_ps_nulls values (1), (null)");
  myquery(rc);

  /* Always memset all members of bind parameter */
  memset(&in_bind, 0, sizeof(in_bind));
  memset(&out_bind, 0, sizeof(out_bind));

  in_bind.buffer_type= MYSQL_TYPE_LONG;
  in_bind.is_null= &in_is_null;
  in_bind.length= 0;
  in_bind.buffer= (void *)&in_long;
  in_is_null= 1;
  in_long= 1;

  out_bind.buffer_type= MYSQL_TYPE_STRING;
  out_bind.is_null= &out_is_null;
  out_bind.length= &out_length;
  out_bind.buffer= out_str_data;
  out_bind.buffer_length= array_elements(out_str_data);

  /* Execute several queries, all returning NULL in result. */
  for(cur_query= queries; *cur_query; cur_query++)
  {
    char query[MAX_TEST_QUERY_LENGTH];
    strmov(query, *cur_query);
    stmt= mysql_simple_prepare(mysql, query);
    check_stmt(stmt);
    verify_param_count(stmt, 1);

    rc= mysql_stmt_bind_param(stmt, &in_bind);
    check_execute(stmt, rc);
    rc= mysql_stmt_bind_result(stmt, &out_bind);
    check_execute(stmt, rc);
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    rc= mysql_stmt_fetch(stmt);
    DIE_UNLESS(rc != MYSQL_NO_DATA);
    DIE_UNLESS(out_is_null);
    rc= mysql_stmt_fetch(stmt);
    DIE_UNLESS(rc == MYSQL_NO_DATA);
    mysql_stmt_close(stmt);
  }
}