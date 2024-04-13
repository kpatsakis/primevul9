static void test_cuted_rows()
{
  int        rc, count;
  MYSQL_RES  *result;

  myheader("test_cuted_rows");

  mysql_query(mysql, "DROP TABLE if exists t1");
  mysql_query(mysql, "DROP TABLE if exists t2");

  rc= mysql_query(mysql, "CREATE TABLE t1(c1 tinyint)");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE t2(c1 int not null)");
  myquery(rc);

  rc= mysql_query(mysql, "INSERT INTO t1 values(10), (NULL), (NULL)");
  myquery(rc);

  count= mysql_warning_count(mysql);
  if (!opt_silent)
    fprintf(stdout, "\n total warnings: %d", count);
  DIE_UNLESS(count == 0);

  rc= mysql_query(mysql, "INSERT INTO t2 SELECT * FROM t1");
  myquery(rc);

  count= mysql_warning_count(mysql);
  if (!opt_silent)
    fprintf(stdout, "\n total warnings: %d", count);
  DIE_UNLESS(count == 2);

  rc= mysql_query(mysql, "SHOW WARNINGS");
  myquery(rc);

  result= mysql_store_result(mysql);
  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS(rc == 2);
  mysql_free_result(result);

  rc= mysql_query(mysql, "INSERT INTO t1 VALUES('junk'), (876789)");
  myquery(rc);

  count= mysql_warning_count(mysql);
  if (!opt_silent)
    fprintf(stdout, "\n total warnings: %d", count);
  DIE_UNLESS(count == 2);

  rc= mysql_query(mysql, "SHOW WARNINGS");
  myquery(rc);

  result= mysql_store_result(mysql);
  mytest(result);

  rc= my_process_result_set(result);
  DIE_UNLESS(rc == 2);
  mysql_free_result(result);
}