static void test_view_insert_fields()
{
  MYSQL_STMT	*stmt;
  char		parm[11][1000];
  ulong         l[11];
  int		rc, i;
  MYSQL_BIND	my_bind[11];
  const char    *query= "INSERT INTO `v1` ( `K1C4` ,`K2C4` ,`K3C4` ,`K4N4` ,`F1C4` ,`F2I4` ,`F3N5` ,`F7F8` ,`F6N4` ,`F5C8` ,`F9D8` ) VALUES( ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? )";

  myheader("test_view_insert_fields");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS t1, v1");
  myquery(rc);
  rc= mysql_query(mysql, "DROP VIEW IF EXISTS t1, v1");
  myquery(rc);
  rc= mysql_query(mysql,
                  "CREATE TABLE t1 (K1C4 varchar(4) NOT NULL,"
                  "K2C4 varchar(4) NOT NULL, K3C4 varchar(4) NOT NULL,"
                  "K4N4 varchar(4) NOT NULL default '0000',"
                  "F1C4 varchar(4) NOT NULL, F2I4 int(11) NOT NULL,"
                  "F3N5 varchar(5) NOT NULL default '00000',"
                  "F4I4 int(11) NOT NULL default '0', F5C8 varchar(8) NOT NULL,"
                  "F6N4 varchar(4) NOT NULL default '0000',"
                  "F7F8 double NOT NULL default '0',"
                  "F8F8 double NOT NULL default '0',"
                  "F9D8 decimal(8,2) NOT NULL default '0.00',"
                  "PRIMARY KEY (K1C4,K2C4,K3C4,K4N4)) "
                  "CHARSET=latin1 COLLATE latin1_bin");
  myquery(rc);
  rc= mysql_query(mysql,
                  "CREATE VIEW v1 AS select sql_no_cache "
                  " K1C4 AS K1C4, K2C4 AS K2C4, K3C4 AS K3C4, K4N4 AS K4N4, "
                  " F1C4 AS F1C4, F2I4 AS F2I4, F3N5 AS F3N5,"
                  " F7F8 AS F7F8, F6N4 AS F6N4, F5C8 AS F5C8, F9D8 AS F9D8"
                  " from t1 T0001");

  memset(my_bind, 0, sizeof(my_bind));
  for (i= 0; i < 11; i++)
  {
    l[i]= 20;
    my_bind[i].buffer_type= MYSQL_TYPE_STRING;
    my_bind[i].is_null= 0;
    my_bind[i].buffer= (char *)&parm[i];

    strmov(parm[i], "1");
    my_bind[i].buffer_length= 2;
    my_bind[i].length= &l[i];
  }
  stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(stmt, query, strlen(query));
  check_execute(stmt, rc);
  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt, rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  mysql_stmt_close(stmt);

  query= "select * from t1";
  stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(stmt, query, strlen(query));
  check_execute(stmt, rc);
  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(1 == rc);

  mysql_stmt_close(stmt);
  rc= mysql_query(mysql, "DROP VIEW v1");
  myquery(rc);
  rc= mysql_query(mysql, "DROP TABLE t1");
  myquery(rc);

}