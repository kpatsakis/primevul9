static void test_mem_overun()
{
  char       buffer[10000], field[10];
  MYSQL_STMT *stmt;
  MYSQL_RES  *field_res;
  int        rc, i, length;

  myheader("test_mem_overun");

  /*
    Test a memory ovverun bug when a table had 1000 fields with
    a row of data
  */
  rc= mysql_query(mysql, "drop table if exists t_mem_overun");
  myquery(rc);

  strxmov(buffer, "create table t_mem_overun(", NullS);
  for (i= 0; i < 1000; i++)
  {
    sprintf(field, "c%d int", i);
    strxmov(buffer, buffer, field, ", ", NullS);
  }
  length= strlen(buffer);
  buffer[length-2]= ')';
  buffer[--length]= '\0';

  rc= mysql_real_query(mysql, buffer, length);
  myquery(rc);

  strxmov(buffer, "insert into t_mem_overun values(", NullS);
  for (i= 0; i < 1000; i++)
  {
    strxmov(buffer, buffer, "1, ", NullS);
  }
  length= strlen(buffer);
  buffer[length-2]= ')';
  buffer[--length]= '\0';

  rc= mysql_real_query(mysql, buffer, length);
  myquery(rc);

  rc= mysql_query(mysql, "select * from t_mem_overun");
  myquery(rc);

  rc= my_process_result(mysql);
  DIE_UNLESS(rc == 1);

  stmt= mysql_simple_prepare(mysql, "select * from t_mem_overun");
  check_stmt(stmt);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  field_res= mysql_stmt_result_metadata(stmt);
  mytest(field_res);

  if (!opt_silent)
    fprintf(stdout, "\n total fields : %d", mysql_num_fields(field_res));
  DIE_UNLESS( 1000 == mysql_num_fields(field_res));

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_free_result(field_res);

  mysql_stmt_close(stmt);
}