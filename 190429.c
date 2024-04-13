static void test_bug11656()
{
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[2];
  int rc;
  const char *stmt_text;
  char buf[2][20];
  int i= 0;
  ulong type;

  myheader("test_bug11656");

  mysql_query(mysql, "drop table if exists t1");

  rc= mysql_query(mysql, "create table t1 ("
                  "server varchar(40) not null, "
                  "test_kind varchar(1) not null, "
                  "test_id varchar(30) not null , "
                  "primary key (server,test_kind,test_id))");
  myquery(rc);

  stmt_text= "select distinct test_kind, test_id from t1 "
             "where server in (?, ?)";
  stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(stmt, stmt_text, strlen(stmt_text));
  check_execute(stmt, rc);
  type= (ulong) CURSOR_TYPE_READ_ONLY;
  mysql_stmt_attr_set(stmt, STMT_ATTR_CURSOR_TYPE, (const void*) &type);

  memset(my_bind, 0, sizeof(my_bind));
  strmov(buf[0], "pcint502_MY2");
  strmov(buf[1], "*");
  for (i=0; i < 2; i++)
  {
    my_bind[i].buffer_type= MYSQL_TYPE_STRING;
    my_bind[i].buffer= (uchar* *)&buf[i];
    my_bind[i].buffer_length= strlen(buf[i]);
  }
  mysql_stmt_bind_param(stmt, my_bind);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= mysql_stmt_fetch(stmt);
  DIE_UNLESS(rc == MYSQL_NO_DATA);

  mysql_stmt_close(stmt);
  rc= mysql_query(mysql, "drop table t1");
  myquery(rc);
}