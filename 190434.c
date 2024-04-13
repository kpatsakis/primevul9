static void test_datetime_ranges()
{
  const char *stmt_text;
  int rc, i;
  MYSQL_STMT *stmt;
  MYSQL_BIND my_bind[6];
  MYSQL_TIME tm[6];

  myheader("test_datetime_ranges");

  stmt_text= "drop table if exists t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "create table t1 (year datetime, month datetime, day datetime, "
                              "hour datetime, min datetime, sec datetime)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt= mysql_simple_prepare(mysql,
                             "INSERT INTO t1 VALUES (?, ?, ?, ?, ?, ?)");
  check_stmt(stmt);
  verify_param_count(stmt, 6);

  memset(my_bind, 0, sizeof(my_bind));
  for (i= 0; i < 6; i++)
  {
    my_bind[i].buffer_type= MYSQL_TYPE_DATETIME;
    my_bind[i].buffer= &tm[i];
  }
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  tm[0].year= 2004; tm[0].month= 11; tm[0].day= 10;
  tm[0].hour= 12; tm[0].minute= 30; tm[0].second= 30;
  tm[0].second_part= 0; tm[0].neg= 0;

  tm[5]= tm[4]= tm[3]= tm[2]= tm[1]= tm[0];
  tm[0].year= 10000;  tm[1].month= 13; tm[2].day= 32;
  tm[3].hour= 24; tm[4].minute= 60; tm[5].second= 60;

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  my_process_warnings(mysql, 12);

  verify_col_data("t1", "year", "0000-00-00 00:00:00");
  verify_col_data("t1", "month", "0000-00-00 00:00:00");
  verify_col_data("t1", "day", "0000-00-00 00:00:00");
  verify_col_data("t1", "hour", "0000-00-00 00:00:00");
  verify_col_data("t1", "min", "0000-00-00 00:00:00");
  verify_col_data("t1", "sec", "0000-00-00 00:00:00");

  mysql_stmt_close(stmt);

  stmt_text= "delete from t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt= mysql_simple_prepare(mysql, "INSERT INTO t1 (year, month, day) "
                                    "VALUES (?, ?, ?)");
  check_stmt(stmt);
  verify_param_count(stmt, 3);

  /*
    We reuse contents of bind and tm arrays left from previous part of test.
  */
  for (i= 0; i < 3; i++)
    my_bind[i].buffer_type= MYSQL_TYPE_DATE;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  my_process_warnings(mysql, 6);

  verify_col_data("t1", "year", "0000-00-00 00:00:00");
  verify_col_data("t1", "month", "0000-00-00 00:00:00");
  verify_col_data("t1", "day", "0000-00-00 00:00:00");

  mysql_stmt_close(stmt);

  stmt_text= "drop table t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt_text= "create table t1 (day_ovfl time, day time, hour time, min time, sec time)";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);

  stmt= mysql_simple_prepare(mysql,
                             "INSERT INTO t1 VALUES (?, ?, ?, ?, ?)");
  check_stmt(stmt);
  verify_param_count(stmt, 5);

  /*
    Again we reuse what we can from previous part of test.
  */
  for (i= 0; i < 5; i++)
    my_bind[i].buffer_type= MYSQL_TYPE_TIME;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  tm[0].year= 0; tm[0].month= 0; tm[0].day= 10;
  tm[0].hour= 12; tm[0].minute= 30; tm[0].second= 30;
  tm[0].second_part= 0; tm[0].neg= 0;

  tm[4]= tm[3]= tm[2]= tm[1]= tm[0];
  tm[0].day= 35; tm[1].day= 34; tm[2].hour= 30; tm[3].minute= 60; tm[4].second= 60;

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  my_process_warnings(mysql, 2);

  verify_col_data("t1", "day_ovfl", "838:59:59");
  verify_col_data("t1", "day", "828:30:30");
  verify_col_data("t1", "hour", "270:30:30");
  verify_col_data("t1", "min", "00:00:00");
  verify_col_data("t1", "sec", "00:00:00");

  mysql_stmt_close(stmt);

  stmt_text= "drop table t1";
  rc= mysql_real_query(mysql, stmt_text, strlen(stmt_text));
  myquery(rc);
}