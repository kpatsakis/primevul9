static void test_long_data_str1()
{
  MYSQL_STMT *stmt;
  int        rc, i;
  char       data[255];
  long       length;
  ulong      max_blob_length, blob_length, length1;
  my_bool    true_value;
  MYSQL_RES  *result;
  MYSQL_BIND my_bind[2];
  MYSQL_FIELD *field;
  char query[MAX_TEST_QUERY_LENGTH];

  myheader("test_long_data_str1");

  rc= mysql_autocommit(mysql, TRUE);
  myquery(rc);

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_long_data_str");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_long_data_str(longstr long varchar, blb long varbinary)");
  myquery(rc);

  strmov(query, "INSERT INTO test_long_data_str VALUES(?, ?)");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 2);

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer= data;            /* string data */
  my_bind[0].buffer_length= sizeof(data);
  my_bind[0].length= &length1;
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  length1= 0;

  my_bind[1]= my_bind[0];
  my_bind[1].buffer_type= MYSQL_TYPE_BLOB;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);
  length= sprintf(data, "MySQL AB");

  /* supply data in pieces */
  for (i= 0; i < 3; i++)
  {
    rc= mysql_stmt_send_long_data(stmt, 0, data, length);
    check_execute(stmt, rc);

    rc= mysql_stmt_send_long_data(stmt, 1, data, 2);
    check_execute(stmt, rc);
  }

  /* execute */
  rc= mysql_stmt_execute(stmt);
  if (!opt_silent)
    fprintf(stdout, " mysql_stmt_execute() returned %d\n", rc);
  check_execute(stmt, rc);

  mysql_stmt_close(stmt);

  rc= mysql_commit(mysql);
  myquery(rc);

  /* now fetch the results ..*/
  rc= mysql_query(mysql, "SELECT LENGTH(longstr), longstr, LENGTH(blb), blb FROM test_long_data_str");
  myquery(rc);

  /* get the result */
  result= mysql_store_result(mysql);

  mysql_field_seek(result, 1);
  field= mysql_fetch_field(result);
  max_blob_length= field->max_length;

  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS(rc == 1);
  mysql_free_result(result);

  sprintf(data, "%ld", (long)i*length);
  verify_col_data("test_long_data_str", "length(longstr)", data);

  sprintf(data, "%d", i*2);
  verify_col_data("test_long_data_str", "length(blb)", data);

  /* Test length of field->max_length */
  stmt= mysql_simple_prepare(mysql, "SELECT * from test_long_data_str");
  check_stmt(stmt);
  verify_param_count(stmt, 0);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  result= mysql_stmt_result_metadata(stmt);
  field= mysql_fetch_fields(result);

  /* First test what happens if STMT_ATTR_UPDATE_MAX_LENGTH is not used */
  DIE_UNLESS(field->max_length == 0);
  mysql_free_result(result);

  /* Enable updating of field->max_length */
  true_value= 1;
  mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, (void*) &true_value);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_store_result(stmt);
  check_execute(stmt, rc);

  result= mysql_stmt_result_metadata(stmt);
  field= mysql_fetch_fields(result);

  DIE_UNLESS(field->max_length == max_blob_length);

  /* Fetch results into a data buffer that is smaller than data */
  memset(my_bind, 0, sizeof(*my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_BLOB;
  my_bind[0].buffer= (void *) &data; /* this buffer won't be altered */
  my_bind[0].buffer_length= 16;
  my_bind[0].length= &blob_length;
  my_bind[0].error= &my_bind[0].error_value;
  rc= mysql_stmt_bind_result(stmt, my_bind);
  data[16]= 0;

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_DATA_TRUNCATED);
  DIE_UNLESS(my_bind[0].error_value);
  DIE_UNLESS(strlen(data) == 16);
  DIE_UNLESS(blob_length == max_blob_length);

  /* Fetch all data */
  memset((my_bind+1), 0, sizeof(*my_bind));
  my_bind[1].buffer_type= MYSQL_TYPE_BLOB;
  my_bind[1].buffer= (void *) &data; /* this buffer won't be altered */
  my_bind[1].buffer_length= sizeof(data);
  my_bind[1].length= &blob_length;
  memset(data, 0, sizeof(data));
  mysql_stmt_fetch_column(stmt, my_bind+1, 0, 0);
  DIE_UNLESS(strlen(data) == max_blob_length);

  mysql_free_result(result);
  mysql_stmt_close(stmt);

  /* Drop created table */
  rc= mysql_query(mysql, "DROP TABLE test_long_data_str");
  myquery(rc);
}