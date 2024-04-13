static void test_date_frac()
{
  int        rc;

  myheader("test_date");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_date");
  myquery(rc);

  rc= mysql_query(mysql, "CREATE TABLE test_date(c1 TIMESTAMP(6), \
                                                 c2 TIME(6), \
                                                 c3 DATETIME(6), \
                                                 c4 DATE)");

  myquery(rc);

  bind_date_conv(5,TRUE);
}