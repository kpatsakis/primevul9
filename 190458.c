static void test_ps_conj_select()
{
  MYSQL_STMT *stmt;
  int        rc;
  MYSQL_BIND my_bind[2];
  int32      int_data;
  char       str_data[32];
  unsigned long str_length;
  char query[MAX_TEST_QUERY_LENGTH];
  myheader("test_ps_conj_select");

  rc= mysql_query(mysql, "drop table if exists t1");
  myquery(rc);

  rc= mysql_query(mysql, "create table t1 (id1 int(11) NOT NULL default '0', "
                         "value2 varchar(100), value1 varchar(100))");
  myquery(rc);

  rc= mysql_query(mysql, "insert into t1 values (1, 'hh', 'hh'), "
                          "(2, 'hh', 'hh'), (1, 'ii', 'ii'), (2, 'ii', 'ii')");
  myquery(rc);

  strmov(query, "select id1, value1 from t1 where id1= ? or "
                "CONVERT(value1 USING utf8)= ?");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 2);

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_LONG;
  my_bind[0].buffer= (void *)&int_data;

  my_bind[1].buffer_type= MYSQL_TYPE_VAR_STRING;
  my_bind[1].buffer= (void *)str_data;
  my_bind[1].buffer_length= array_elements(str_data);
  my_bind[1].length= &str_length;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  int_data= 1;
  strmov(str_data, "hh");
  str_length= strlen(str_data);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(rc == 3);

  mysql_stmt_close(stmt);
}