void do_warn(SV* h, int rc, char* what)
{
  dTHX;
  D_imp_xxh(h);

  SV *errstr = DBIc_ERRSTR(imp_xxh);
  sv_setiv(DBIc_ERR(imp_xxh), (IV)rc);	/* set err early	*/
  sv_setpv(errstr, what);
  /* NO EFFECT DBIh_EVENT2(h, WARN_event, DBIc_ERR(imp_xxh), errstr);*/
  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "%s warning %d recorded: %s\n",
    what, rc, SvPV_nolen(errstr));
  warn("%s", what);
}