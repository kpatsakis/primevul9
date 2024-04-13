static void test_view()
{
  MYSQL_STMT *stmt;
  int rc, i;
  MYSQL_BIND      my_bind[1];
  char            str_data[50];
  ulong           length = 0L;
  long            is_null = 0L;
  const char *query=
    "SELECT COUNT(*) FROM v1 WHERE SERVERNAME=?";

  myheader("test_view");

  rc = mysql_query(mysql, "DROP TABLE IF EXISTS t1,t2,t3,v1");
  myquery(rc);

  rc = mysql_query(mysql, "DROP VIEW IF EXISTS v1,t1,t2,t3");
  myquery(rc);
  rc= mysql_query(mysql,"CREATE TABLE t1 ("
                        " SERVERGRP varchar(20) NOT NULL default '', "
                        " DBINSTANCE varchar(20) NOT NULL default '', "
                        " PRIMARY KEY  (SERVERGRP)) "
                        " CHARSET=latin1 collate=latin1_bin");
  myquery(rc);
  rc= mysql_query(mysql,"CREATE TABLE t2 ("
                        " SERVERNAME varchar(20) NOT NULL, "
                        " SERVERGRP varchar(20) NOT NULL, "
                        " PRIMARY KEY (SERVERNAME)) "
                        " CHARSET=latin1 COLLATE latin1_bin");
  myquery(rc);
  rc= mysql_query(mysql,
                  "CREATE TABLE t3 ("
                  " SERVERGRP varchar(20) BINARY NOT NULL, "
                  " TABNAME varchar(30) NOT NULL, MAPSTATE char(1) NOT NULL, "
                  " ACTSTATE char(1) NOT NULL , "
                  " LOCAL_NAME varchar(30) NOT NULL, "
                  " CHG_DATE varchar(8) NOT NULL default '00000000', "
                  " CHG_TIME varchar(6) NOT NULL default '000000', "
                  " MXUSER varchar(12) NOT NULL default '', "
                  " PRIMARY KEY (SERVERGRP, TABNAME, MAPSTATE, ACTSTATE, "
                  " LOCAL_NAME)) CHARSET=latin1 COLLATE latin1_bin");
  myquery(rc);
  rc= mysql_query(mysql,"CREATE VIEW v1 AS select sql_no_cache"
                  " T0001.SERVERNAME AS SERVERNAME, T0003.TABNAME AS"
                  " TABNAME,T0003.LOCAL_NAME AS LOCAL_NAME,T0002.DBINSTANCE AS"
                  " DBINSTANCE from t2 T0001 join t1 T0002 join t3 T0003 where"
                  " ((T0002.SERVERGRP = T0001.SERVERGRP) and"
                  " (T0002.SERVERGRP = T0003.SERVERGRP)"
                  " and (T0003.MAPSTATE = _latin1'A') and"
                  " (T0003.ACTSTATE = _latin1' '))");
  myquery(rc);

  stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(stmt, query, strlen(query));
  check_execute(stmt, rc);

  strmov(str_data, "TEST");
  memset(my_bind, 0, sizeof(my_bind));
  my_bind[0].buffer_type= MYSQL_TYPE_STRING;
  my_bind[0].buffer= (char *)&str_data;
  my_bind[0].buffer_length= 50;
  my_bind[0].length= &length;
  length= 4;
  my_bind[0].is_null= (char*)&is_null;
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt,rc);

  for (i= 0; i < 3; i++)
  {
    rc= mysql_stmt_execute(stmt);
    check_execute(stmt, rc);
    rc= my_process_stmt_result(stmt);
    DIE_UNLESS(1 == rc);
  }
  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "DROP TABLE t1,t2,t3");
  myquery(rc);
  rc= mysql_query(mysql, "DROP VIEW v1");
  myquery(rc);
}