dbd_db_STORE_attrib(
                    SV* dbh,
                    imp_dbh_t* imp_dbh,
                    SV* keysv,
                    SV* valuesv
                   )
{
  dTHX;
  STRLEN kl;
  char *key = SvPV(keysv, kl);
  SV *cachesv = Nullsv;
  int cacheit = FALSE;
  const bool bool_value = SvTRUE(valuesv);

  if (kl==10 && strEQ(key, "AutoCommit"))
  {
    if (imp_dbh->has_transactions)
    {
      bool oldval = DBIc_has(imp_dbh,DBIcf_AutoCommit) ? 1 : 0;

      if (bool_value == oldval)
        return TRUE;

      /* if setting AutoCommit on ... */
      if (!imp_dbh->no_autocommit_cmd)
      {
        if (
#if MYSQL_VERSION_ID >=SERVER_PREPARE_VERSION
            mysql_autocommit(imp_dbh->pmysql, bool_value)
#else
            mysql_real_query(imp_dbh->pmysql,
                             bool_value ? "SET AUTOCOMMIT=1" : "SET AUTOCOMMIT=0",
                             16)
#endif
           )
        {
          do_error(dbh, TX_ERR_AUTOCOMMIT,
                   bool_value ?
                   "Turning on AutoCommit failed" :
                   "Turning off AutoCommit failed"
                   ,NULL);
          return TRUE;  /* TRUE means we handled it - important to avoid spurious errors */
        }
      }
      DBIc_set(imp_dbh, DBIcf_AutoCommit, bool_value);
    }
    else
    {
      /*
       *  We do support neither transactions nor "AutoCommit".
       *  But we stub it. :-)
      */
      if (!bool_value)
      {
        do_error(dbh, JW_ERR_NOT_IMPLEMENTED,
                 "Transactions not supported by database" ,NULL);
        croak("Transactions not supported by database");
      }
    }
  }
  else if (kl == 16 && strEQ(key,"mysql_use_result"))
    imp_dbh->use_mysql_use_result = bool_value;
  else if (kl == 20 && strEQ(key,"mysql_auto_reconnect"))
    imp_dbh->auto_reconnect = bool_value;
  else if (kl == 20 && strEQ(key, "mysql_server_prepare"))
    imp_dbh->use_server_side_prepare = bool_value;
  else if (kl == 37 && strEQ(key, "mysql_server_prepare_disable_fallback"))
    imp_dbh->disable_fallback_for_server_prepare = bool_value;
  else if (kl == 23 && strEQ(key,"mysql_no_autocommit_cmd"))
    imp_dbh->no_autocommit_cmd = bool_value;
  else if (kl == 24 && strEQ(key,"mysql_bind_type_guessing"))
    imp_dbh->bind_type_guessing = bool_value;
  else if (kl == 31 && strEQ(key,"mysql_bind_comment_placeholders"))
    imp_dbh->bind_type_guessing = bool_value;
#if defined(sv_utf8_decode) && MYSQL_VERSION_ID >=SERVER_PREPARE_VERSION
  else if (kl == 17 && strEQ(key, "mysql_enable_utf8"))
    imp_dbh->enable_utf8 = bool_value;
  else if (kl == 20 && strEQ(key, "mysql_enable_utf8mb4"))
    imp_dbh->enable_utf8mb4 = bool_value;
#endif
#if FABRIC_SUPPORT
  else if (kl == 22 && strEQ(key, "mysql_fabric_opt_group"))
    mysql_options(imp_dbh->pmysql, FABRIC_OPT_GROUP, (void *)SvPVbyte_nolen(valuesv));
  else if (kl == 29 && strEQ(key, "mysql_fabric_opt_default_mode"))
  {
    if (SvOK(valuesv)) {
      STRLEN len;
      const char *str = SvPVbyte(valuesv, len);
      if ( len == 0 || ( len == 2 && (strnEQ(str, "ro", 3) || strnEQ(str, "rw", 3)) ) )
        mysql_options(imp_dbh->pmysql, FABRIC_OPT_DEFAULT_MODE, len == 0 ? NULL : str);
      else
        croak("Valid settings for FABRIC_OPT_DEFAULT_MODE are 'ro', 'rw', or undef/empty string");
    }
    else {
      mysql_options(imp_dbh->pmysql, FABRIC_OPT_DEFAULT_MODE, NULL);
    }
  }
  else if (kl == 21 && strEQ(key, "mysql_fabric_opt_mode"))
  {
    STRLEN len;
    const char *str = SvPVbyte(valuesv, len);
    if (len != 2 || (strnNE(str, "ro", 3) && strnNE(str, "rw", 3)))
      croak("Valid settings for FABRIC_OPT_MODE are 'ro' or 'rw'");

    mysql_options(imp_dbh->pmysql, FABRIC_OPT_MODE, str);
  }
  else if (kl == 34 && strEQ(key, "mysql_fabric_opt_group_credentials"))
  {
    croak("'fabric_opt_group_credentials' is not supported");
  }
#endif
  else
    return FALSE;				/* Unknown key */

  if (cacheit) /* cache value for later DBI 'quick' fetch? */
    (void)hv_store((HV*)SvRV(dbh), key, kl, cachesv, 0);
  return TRUE;
}