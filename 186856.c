static int my_login(pTHX_ SV* dbh, imp_dbh_t *imp_dbh)
{
  SV* sv;
  HV* hv;
  char* dbname;
  char* host;
  char* port;
  char* user;
  char* password;
  char* mysql_socket;
  int   result;
  int fresh = 0;
  D_imp_xxh(dbh);

  /* TODO- resolve this so that it is set only if DBI is 1.607 */
#define TAKE_IMP_DATA_VERSION 1
#if TAKE_IMP_DATA_VERSION
  if (DBIc_has(imp_dbh, DBIcf_IMPSET))
  { /* eg from take_imp_data() */
    if (DBIc_has(imp_dbh, DBIcf_ACTIVE))
    {
      if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
        PerlIO_printf(DBIc_LOGPIO(imp_xxh), "my_login skip connect\n");
      /* tell our parent we've adopted an active child */
      ++DBIc_ACTIVE_KIDS(DBIc_PARENT_COM(imp_dbh));
      return TRUE;
    }
    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                    "my_login IMPSET but not ACTIVE so connect not skipped\n");
  }
#endif

  sv = DBIc_IMP_DATA(imp_dbh);

  if (!sv  ||  !SvROK(sv))
    return FALSE;

  hv = (HV*) SvRV(sv);
  if (SvTYPE(hv) != SVt_PVHV)
    return FALSE;

  host=		safe_hv_fetch(aTHX_ hv, "host", 4);
  port=		safe_hv_fetch(aTHX_ hv, "port", 4);
  user=		safe_hv_fetch(aTHX_ hv, "user", 4);
  password=	safe_hv_fetch(aTHX_ hv, "password", 8);
  dbname=	safe_hv_fetch(aTHX_ hv, "database", 8);
  mysql_socket=	safe_hv_fetch(aTHX_ hv, "mysql_socket", 12);

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
		  "imp_dbh->my_login : dbname = %s, uid = %s, pwd = %s," \
		  "host = %s, port = %s\n",
		  dbname ? dbname : "NULL",
		  user ? user : "NULL",
		  password ? password : "NULL",
		  host ? host : "NULL",
		  port ? port : "NULL");

  if (!imp_dbh->pmysql) {
     fresh = 1;
     Newz(908, imp_dbh->pmysql, 1, MYSQL);
  }
  result = mysql_dr_connect(dbh, imp_dbh->pmysql, mysql_socket, host, port, user,
			  password, dbname, imp_dbh) ? TRUE : FALSE;
  return result;
}