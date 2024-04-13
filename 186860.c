SV* dbd_db_FETCH_attrib(SV *dbh, imp_dbh_t *imp_dbh, SV *keysv)
{
  dTHX;
  STRLEN kl;
  char *key = SvPV(keysv, kl);
  char* fine_key = NULL;
  SV* result = NULL;
  dbh= dbh;

  switch (*key) {
    case 'A':
      if (strEQ(key, "AutoCommit"))
      {
        if (imp_dbh->has_transactions)
          return sv_2mortal(boolSV(DBIc_has(imp_dbh,DBIcf_AutoCommit)));
        /* Default */
        return &PL_sv_yes;
      }
      break;
  }
  if (strncmp(key, "mysql_", 6) == 0) {
    fine_key = key;
    key = key+6;
    kl = kl-6;
  }

  /* MONTY:  Check if kl should not be used or used everywhere */
  switch(*key) {
  case 'a':
    if (kl == strlen("auto_reconnect") && strEQ(key, "auto_reconnect"))
      result= sv_2mortal(newSViv(imp_dbh->auto_reconnect));
    break;
  case 'b':
    if (kl == strlen("bind_type_guessing") &&
        strEQ(key, "bind_type_guessing"))
    {
      result = sv_2mortal(newSViv(imp_dbh->bind_type_guessing));
    }
    else if (kl == strlen("bind_comment_placeholders") &&
        strEQ(key, "bind_comment_placeholders"))
    {
      result = sv_2mortal(newSViv(imp_dbh->bind_comment_placeholders));
    }
    break;
  case 'c':
    if (kl == 10 && strEQ(key, "clientinfo"))
    {
      const char* clientinfo = mysql_get_client_info();
      result= clientinfo ?
        sv_2mortal(newSVpv(clientinfo, strlen(clientinfo))) : &PL_sv_undef;
    }
    else if (kl == 13 && strEQ(key, "clientversion"))
    {
      result= sv_2mortal(my_ulonglong2str(aTHX_ mysql_get_client_version()));
    }
    break;
  case 'e':
    if (strEQ(key, "errno"))
      result= sv_2mortal(newSViv((IV)mysql_errno(imp_dbh->pmysql)));
    else if ( strEQ(key, "error") || strEQ(key, "errmsg"))
    {
    /* Note that errmsg is obsolete, as of 2.09! */
      const char* msg = mysql_error(imp_dbh->pmysql);
      result= sv_2mortal(newSVpv(msg, strlen(msg)));
    }
    /* HELMUT */
#if defined(sv_utf8_decode) && MYSQL_VERSION_ID >=SERVER_PREPARE_VERSION
    else if (kl == strlen("enable_utf8mb4") && strEQ(key, "enable_utf8mb4"))
        result = sv_2mortal(newSViv(imp_dbh->enable_utf8mb4));
    else if (kl == strlen("enable_utf8") && strEQ(key, "enable_utf8"))
        result = sv_2mortal(newSViv(imp_dbh->enable_utf8));
#endif
    break;

  case 'd':
    if (strEQ(key, "dbd_stats"))
    {
      HV* hv = newHV();
      hv_store(
               hv,
               "auto_reconnects_ok",
               strlen("auto_reconnects_ok"),
               newSViv(imp_dbh->stats.auto_reconnects_ok),
               0
              );
      hv_store(
               hv,
               "auto_reconnects_failed",
               strlen("auto_reconnects_failed"),
               newSViv(imp_dbh->stats.auto_reconnects_failed),
               0
              );

      result= sv_2mortal((newRV_noinc((SV*)hv)));
    }

  case 'h':
    if (strEQ(key, "hostinfo"))
    {
      const char* hostinfo = mysql_get_host_info(imp_dbh->pmysql);
      result= hostinfo ?
        sv_2mortal(newSVpv(hostinfo, strlen(hostinfo))) : &PL_sv_undef;
    }
    break;

  case 'i':
    if (strEQ(key, "info"))
    {
      const char* info = mysql_info(imp_dbh->pmysql);
      result= info ? sv_2mortal(newSVpv(info, strlen(info))) : &PL_sv_undef;
    }
    else if (kl == 8  &&  strEQ(key, "insertid"))
      /* We cannot return an IV, because the insertid is a long. */
      result= sv_2mortal(my_ulonglong2str(aTHX_ mysql_insert_id(imp_dbh->pmysql)));
    break;
  case 'n':
    if (kl == strlen("no_autocommit_cmd") &&
        strEQ(key, "no_autocommit_cmd"))
      result = sv_2mortal(newSViv(imp_dbh->no_autocommit_cmd));
    break;

  case 'p':
    if (kl == 9  &&  strEQ(key, "protoinfo"))
      result= sv_2mortal(newSViv(mysql_get_proto_info(imp_dbh->pmysql)));
    break;

  case 's':
    if (kl == 10 && strEQ(key, "serverinfo")) {
      const char* serverinfo = mysql_get_server_info(imp_dbh->pmysql);
      result= serverinfo ?
        sv_2mortal(newSVpv(serverinfo, strlen(serverinfo))) : &PL_sv_undef;
    } 
    else if (kl == 13 && strEQ(key, "serverversion"))
      result= sv_2mortal(my_ulonglong2str(aTHX_ mysql_get_server_version(imp_dbh->pmysql)));
    else if (strEQ(key, "sock"))
      result= sv_2mortal(newSViv((IV) imp_dbh->pmysql));
    else if (strEQ(key, "sockfd"))
      result= sv_2mortal(newSViv((IV) imp_dbh->pmysql->net.fd));
    else if (strEQ(key, "stat"))
    {
      const char* stats = mysql_stat(imp_dbh->pmysql);
      result= stats ?
        sv_2mortal(newSVpv(stats, strlen(stats))) : &PL_sv_undef;
    }
    else if (strEQ(key, "stats"))
    {
      /* Obsolete, as of 2.09 */
      const char* stats = mysql_stat(imp_dbh->pmysql);
      result= stats ?
        sv_2mortal(newSVpv(stats, strlen(stats))) : &PL_sv_undef;
    }
    else if (kl == 14 && strEQ(key,"server_prepare"))
        result= sv_2mortal(newSViv((IV) imp_dbh->use_server_side_prepare));
    break;

  case 't':
    if (kl == 9  &&  strEQ(key, "thread_id"))
      result= sv_2mortal(newSViv(mysql_thread_id(imp_dbh->pmysql)));
    break;

  case 'w':
    if (kl == 13 && strEQ(key, "warning_count"))
      result= sv_2mortal(newSViv(mysql_warning_count(imp_dbh->pmysql)));
    break;
  case 'u':
    if (strEQ(key, "use_result"))
    {
      result= sv_2mortal(newSViv((IV) imp_dbh->use_mysql_use_result));
    }
    break;
  }

  if (result== NULL)
    return Nullsv;

  return result;
}