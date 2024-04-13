static void test_bug56976()
{
  MYSQL_STMT   *stmt;
  MYSQL_BIND    bind[1];
  int           rc;
  const char*   query = "SELECT LENGTH(?)";
  char *long_buffer;
  unsigned long i, packet_len = 256 * 1024L;
  unsigned long dos_len    = 8 * 1024 * 1024L;

  DBUG_ENTER("test_bug56976");
  myheader("test_bug56976");

  stmt= mysql_stmt_init(mysql);
  check_stmt(stmt);

  rc= mysql_stmt_prepare(stmt, query, strlen(query));
  check_execute(stmt, rc);

  memset(bind, 0, sizeof(bind));
  bind[0].buffer_type = MYSQL_TYPE_TINY_BLOB;

  rc= mysql_stmt_bind_param(stmt, bind);
  check_execute(stmt, rc);

  long_buffer= (char*) my_malloc(packet_len, MYF(0));
  DIE_UNLESS(long_buffer);

  memset(long_buffer, 'a', packet_len);

  for (i= 0; i < dos_len / packet_len; i++)
  {
    rc= mysql_stmt_send_long_data(stmt, 0, long_buffer, packet_len);
    check_execute(stmt, rc);
  }

  my_free(long_buffer);
  rc= mysql_stmt_execute(stmt);

  DIE_UNLESS(rc && mysql_stmt_errno(stmt) == ER_UNKNOWN_ERROR);

  mysql_stmt_close(stmt);

  DBUG_VOID_RETURN;
}