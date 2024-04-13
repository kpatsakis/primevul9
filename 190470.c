static void test_view_2where()
{
  MYSQL_STMT *stmt;
  int rc, i;
  MYSQL_BIND      my_bind[8];
  char            parms[8][100];
  ulong           length[8];
  const char *query=
    "select relid, report, handle, log_group, username, variant, type, "
    "version, erfdat, erftime, erfname, aedat, aetime, aename, dependvars, "
    "inactive from V_LTDX where mandt = ? and relid = ? and report = ? and "
    "handle = ? and log_group = ? and username in ( ? , ? ) and type = ?";

  myheader("test_view_2where");

  rc= mysql_query(mysql, "DROP TABLE IF EXISTS LTDX");
  myquery(rc);
  rc= mysql_query(mysql, "DROP VIEW IF EXISTS V_LTDX");
  myquery(rc);
  rc= mysql_query(mysql,
                  "CREATE TABLE LTDX (MANDT char(3) NOT NULL default '000', "
                  " RELID char(2) NOT NULL, REPORT varchar(40) NOT NULL,"
                  " HANDLE varchar(4) NOT NULL, LOG_GROUP varchar(4) NOT NULL,"
                  " USERNAME varchar(12) NOT NULL,"
                  " VARIANT varchar(12) NOT NULL,"
                  " TYPE char(1) NOT NULL, SRTF2 int(11) NOT NULL,"
                  " VERSION varchar(6) NOT NULL default '000000',"
                  " ERFDAT varchar(8) NOT NULL default '00000000',"
                  " ERFTIME varchar(6) NOT NULL default '000000',"
                  " ERFNAME varchar(12) NOT NULL,"
                  " AEDAT varchar(8) NOT NULL default '00000000',"
                  " AETIME varchar(6) NOT NULL default '000000',"
                  " AENAME varchar(12) NOT NULL,"
                  " DEPENDVARS varchar(10) NOT NULL,"
                  " INACTIVE char(1) NOT NULL, CLUSTR smallint(6) NOT NULL,"
                  " CLUSTD blob,"
                  " PRIMARY KEY (MANDT, RELID, REPORT, HANDLE, LOG_GROUP, "
                                "USERNAME, VARIANT, TYPE, SRTF2))"
                 " CHARSET=latin1 COLLATE latin1_bin");
  myquery(rc);
  rc= mysql_query(mysql,
                  "CREATE VIEW V_LTDX AS select T0001.MANDT AS "
                  " MANDT,T0001.RELID AS RELID,T0001.REPORT AS "
                  " REPORT,T0001.HANDLE AS HANDLE,T0001.LOG_GROUP AS "
                  " LOG_GROUP,T0001.USERNAME AS USERNAME,T0001.VARIANT AS "
                  " VARIANT,T0001.TYPE AS TYPE,T0001.VERSION AS "
                  " VERSION,T0001.ERFDAT AS ERFDAT,T0001.ERFTIME AS "
                  " ERFTIME,T0001.ERFNAME AS ERFNAME,T0001.AEDAT AS "
                  " AEDAT,T0001.AETIME AS AETIME,T0001.AENAME AS "
                  " AENAME,T0001.DEPENDVARS AS DEPENDVARS,T0001.INACTIVE AS "
                  " INACTIVE from LTDX T0001 where (T0001.SRTF2 = 0)");
  myquery(rc);
  memset(my_bind, 0, sizeof(my_bind));
  for (i=0; i < 8; i++) {
    strmov(parms[i], "1");
    my_bind[i].buffer_type = MYSQL_TYPE_VAR_STRING;
    my_bind[i].buffer = (char *)&parms[i];
    my_bind[i].buffer_length = 100;
    my_bind[i].is_null = 0;
    my_bind[i].length = &length[i];
    length[i] = 1;
  }
  stmt= mysql_stmt_init(mysql);
  rc= mysql_stmt_prepare(stmt, query, strlen(query));
  check_execute(stmt, rc);

  rc= mysql_stmt_bind_param(stmt, my_bind);
  check_execute(stmt,rc);

  rc= mysql_stmt_execute(stmt);
  check_execute(stmt, rc);
  rc= my_process_stmt_result(stmt);
  DIE_UNLESS(0 == rc);

  mysql_stmt_close(stmt);

  rc= mysql_query(mysql, "DROP VIEW V_LTDX");
  myquery(rc);
  rc= mysql_query(mysql, "DROP TABLE LTDX");
  myquery(rc);
}