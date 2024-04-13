static void test_prepare_ext()
{
  MYSQL_STMT *stmt;
  int        rc;
  char       *sql;
  int        nData= 1;
  char       tData= 1;
  short      sData= 10;
  longlong   bData= 20;
  MYSQL_BIND my_bind[6];
  char query[MAX_TEST_QUERY_LENGTH];
  myheader("test_prepare_ext");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS test_prepare_ext");
  myquery(rc);

  sql= (char *)"CREATE TABLE test_prepare_ext"
               "("
               " c1  tinyint,"
               " c2  smallint,"
               " c3  mediumint,"
               " c4  int,"
               " c5  integer,"
               " c6  bigint,"
               " c7  float,"
               " c8  double,"
               " c9  double precision,"
               " c10 real,"
               " c11 decimal(7, 4),"
               " c12 numeric(8, 4),"
               " c13 date,"
               " c14 datetime,"
               " c15 timestamp,"
               " c16 time,"
               " c17 year,"
               " c18 bit,"
               " c19 bool,"
               " c20 char,"
               " c21 char(10),"
               " c22 varchar(30),"
               " c23 tinyblob,"
               " c24 tinytext,"
               " c25 blob,"
               " c26 text,"
               " c27 mediumblob,"
               " c28 mediumtext,"
               " c29 longblob,"
               " c30 longtext,"
               " c31 enum('one', 'two', 'three'),"
               " c32 set('monday', 'tuesday', 'wednesday'))";

  rc= mysql_query(mysql, sql);
  myquery(rc);

  /* insert by prepare - all integers */
  strmov(query, (char *)"INSERT INTO test_prepare_ext(c1, c2, c3, c4, c5, c6) VALUES(?, ?, ?, ?, ?, ?)");
  stmt= mysql_simple_prepare(mysql, query);
  check_stmt(stmt);

  verify_param_count(stmt, 6);

  /* Always memset all members of bind parameter */
  memset(my_bind, 0, sizeof(my_bind));

  /*tinyint*/
  my_bind[0].buffer_type= MYSQL_TYPE_TINY;
  my_bind[0].buffer= (void *)&tData;

  /*smallint*/
  my_bind[1].buffer_type= MYSQL_TYPE_SHORT;
  my_bind[1].buffer= (void *)&sData;

  /*mediumint*/
  my_bind[2].buffer_type= MYSQL_TYPE_LONG;
  my_bind[2].buffer= (void *)&nData;

  /*int*/
  my_bind[3].buffer_type= MYSQL_TYPE_LONG;
  my_bind[3].buffer= (void *)&nData;

  /*integer*/
  my_bind[4].buffer_type= MYSQL_TYPE_LONG;
  my_bind[4].buffer= (void *)&nData;

  /*bigint*/
  my_bind[5].buffer_type= MYSQL_TYPE_LONGLONG;
  my_bind[5].buffer= (void *)&bData;

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  /*
  *  integer to integer
  */
  for (nData= 0; nData<10; nData++, tData++, sData++, bData++)
  {
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
  }
  mysql_stmt_close(stmt);

  /* now fetch the results ..*/

  stmt= mysql_simple_prepare(mysql, "SELECT c1, c2, c3, c4, c5, c6 "
                                    "FROM test_prepare_ext");
  check_stmt(stmt);

  /* get the result */
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);

  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(nData == rc);

  mysql_stmt_close(stmt);
}