void do_error(SV* h, int rc, const char* what, const char* sqlstate)
{
  dTHX;
  D_imp_xxh(h);
  SV *errstr;
  SV *errstate;

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\t\t--> do_error\n");
  errstr= DBIc_ERRSTR(imp_xxh);
  sv_setiv(DBIc_ERR(imp_xxh), (IV)rc);	/* set err early	*/
  sv_setpv(errstr, what);

#if MYSQL_VERSION_ID >= SQL_STATE_VERSION
  if (sqlstate)
  {
    errstate= DBIc_STATE(imp_xxh);
    sv_setpvn(errstate, sqlstate, 5);
  }
#endif

  /* NO EFFECT DBIh_EVENT2(h, ERROR_event, DBIc_ERR(imp_xxh), errstr); */
  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "%s error %d recorded: %s\n",
    what, rc, SvPV_nolen(errstr));
  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\t\t<-- do_error\n");
}