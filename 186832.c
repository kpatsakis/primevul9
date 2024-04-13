int dbd_discon_all (SV *drh, imp_drh_t *imp_drh) {
#if defined(dTHR)
  dTHR;
#endif
  dTHX;
#if defined(DBD_MYSQL_EMBEDDED)
  D_imp_xxh(drh);
#else
  PERL_UNUSED_ARG(drh);
#endif

#if defined(DBD_MYSQL_EMBEDDED)
  if (imp_drh->embedded.state)
  {
    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      PerlIO_printf(DBIc_LOGPIO(imp_xxh), "Stop embedded server\n");

    mysql_server_end();
    if (imp_drh->embedded.groups)
    {
      (void) SvREFCNT_dec(imp_drh->embedded.groups);
      imp_drh->embedded.groups = NULL;
    }

    if (imp_drh->embedded.args)
    {
      (void) SvREFCNT_dec(imp_drh->embedded.args);
      imp_drh->embedded.args = NULL;
    }


  }
#else
  mysql_server_end();
#endif

  /* The disconnect_all concept is flawed and needs more work */
  if (!PL_dirty && !SvTRUE(perl_get_sv("DBI::PERL_ENDING",0))) {
    sv_setiv(DBIc_ERR(imp_drh), (IV)1);
    sv_setpv(DBIc_ERRSTR(imp_drh),
             (char*)"disconnect_all not implemented");
    /* NO EFFECT DBIh_EVENT2(drh, ERROR_event,
      DBIc_ERR(imp_drh), DBIc_ERRSTR(imp_drh)); */
    return FALSE;
  }
  PL_perl_destruct_level = 0;
  return FALSE;
}