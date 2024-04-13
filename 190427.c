static void test_bug3796()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[1];
  const char *concat_arg0= "concat_with_";
  enum { OUT_BUFF_SIZE= 30 };
  char out_buff[OUT_BUFF_SIZE];
  char canonical_buff[OUT_BUFF_SIZE];
  ulong out_length;
  const char *stmt_text;
  int rc;

  myheader("test_bug3796");

  /* Create and fill test table */
  stmt_text= "DROP TABLE IF EXISTS t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "CREATE TABLE t1 (a INT, b VARCHAR(30))";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "INSERT INTO t1 VALUES(1, 'ONE'), (2, 'TWO')";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  /* Create statement handle and prepare it with select */
  stmt= mysql_stmt_init(mysql);
  stmt_text= "SELECT concat(?, b) FROM t1";

  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);

  /* Bind input buffers */
  memset(my_bind, 0, sizeof(my_bind));

  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (void *) concat_arg0;
  my_bind[0].buffer_length= strlen(concat_arg0);

  mysql_stmt_bind_param(stmt, my_bind);

  /* Execute the select statement */
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  my_bind[0].buffer= (void *) out_buff;
  my_bind[0].buffer_length= OUT_BUFF_SIZE;
  my_bind[0].length= &out_length;

  mysql_stmt_bind_result(stmt, my_bind);

  rc= mysql_stmt_fetch(stmt);
  if (!opt_silent)
    printf("Concat result: '%s'\n", out_buff);
  check_execute(stmt, rc);
  strmov(canonical_buff, concat_arg0);
  strcat(canonical_buff, "ONE");
  DIE_UNLESS(strlen(canonical_buff) == out_length &&
         strncmp(out_buff, canonical_buff, out_length) == 0);

  rc= mysql_stmt_fetch(stmt);
  check_execute(stmt, rc);
  strmov(canonical_buff + strlen(concat_arg0), "TWO");
  DIE_UNLESS(strlen(canonical_buff) == out_length &&
         strncmp(out_buff, canonical_buff, out_length) == 0);
  if (!opt_silent)
    printf("Concat result: '%s'\n", out_buff);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);

  stmt_text= "DROP TABLE IF EXISTS t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
}