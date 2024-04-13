  SV* dbd_st_FETCH_attrib(
                          SV *sth,
                          imp_sth_t *imp_sth,
                          SV *keysv
                         )
{
  dTHX;
  STRLEN(kl);
  char *key= SvPV(keysv, kl);
  SV *retsv= Nullsv;
  D_imp_xxh(sth);

  if (kl < 2)
    return Nullsv;

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                  "    -> dbd_st_FETCH_attrib for %p, key %s\n",
                  sth, key);

  switch (*key) {
  case 'N':
    if (strEQ(key, "NAME"))
      retsv= ST_FETCH_AV(AV_ATTRIB_NAME);
    else if (strEQ(key, "NULLABLE"))
      retsv= ST_FETCH_AV(AV_ATTRIB_NULLABLE);
    break;
  case 'P':
    if (strEQ(key, "PRECISION"))
      retsv= ST_FETCH_AV(AV_ATTRIB_PRECISION);
    if (strEQ(key, "ParamValues"))
    {
        HV *pvhv= newHV();
        if (DBIc_NUM_PARAMS(imp_sth))
        {
            int n;
            char key[100];
            I32 keylen;
            for (n= 0; n < DBIc_NUM_PARAMS(imp_sth); n++)
            {
                keylen= sprintf(key, "%d", n);
                (void)hv_store(pvhv, key,
                         keylen, newSVsv(imp_sth->params[n].value), 0);
            }
        }
        retsv= sv_2mortal(newRV_noinc((SV*)pvhv));
    }
    break;
  case 'S':
    if (strEQ(key, "SCALE"))
      retsv= ST_FETCH_AV(AV_ATTRIB_SCALE);
    break;
  case 'T':
    if (strEQ(key, "TYPE"))
      retsv= ST_FETCH_AV(AV_ATTRIB_SQL_TYPE);
    break;
  case 'm':
    switch (kl) {
    case 10:
      if (strEQ(key, "mysql_type"))
        retsv= ST_FETCH_AV(AV_ATTRIB_TYPE);
      break;
    case 11:
      if (strEQ(key, "mysql_table"))
        retsv= ST_FETCH_AV(AV_ATTRIB_TABLE);
      break;
    case 12:
      if (       strEQ(key, "mysql_is_key"))
        retsv= ST_FETCH_AV(AV_ATTRIB_IS_KEY);
      else if (strEQ(key, "mysql_is_num"))
        retsv= ST_FETCH_AV(AV_ATTRIB_IS_NUM);
      else if (strEQ(key, "mysql_length"))
        retsv= ST_FETCH_AV(AV_ATTRIB_LENGTH);
      else if (strEQ(key, "mysql_result"))
        retsv= sv_2mortal(newSViv(PTR2IV(imp_sth->result)));
      break;
    case 13:
      if (strEQ(key, "mysql_is_blob"))
        retsv= ST_FETCH_AV(AV_ATTRIB_IS_BLOB);
      break;
    case 14:
      if (strEQ(key, "mysql_insertid"))
      {
        /* We cannot return an IV, because the insertid is a long.  */
        if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
          PerlIO_printf(DBIc_LOGPIO(imp_xxh), "INSERT ID %llu\n", imp_sth->insertid);

        return sv_2mortal(my_ulonglong2str(aTHX_ imp_sth->insertid));
      }
      break;
    case 15:
      if (strEQ(key, "mysql_type_name"))
        retsv = ST_FETCH_AV(AV_ATTRIB_TYPE_NAME);
      break;
    case 16:
      if ( strEQ(key, "mysql_is_pri_key"))
        retsv= ST_FETCH_AV(AV_ATTRIB_IS_PRI_KEY);
      else if (strEQ(key, "mysql_max_length"))
        retsv= ST_FETCH_AV(AV_ATTRIB_MAX_LENGTH);
      else if (strEQ(key, "mysql_use_result"))
        retsv= boolSV(imp_sth->use_mysql_use_result);
      break;
    case 19:
      if (strEQ(key, "mysql_warning_count"))
        retsv= sv_2mortal(newSViv((IV) imp_sth->warning_count));
      break;
    case 20:
      if (strEQ(key, "mysql_server_prepare"))
#if MYSQL_VERSION_ID >= SERVER_PREPARE_VERSION
        retsv= sv_2mortal(newSViv((IV) imp_sth->use_server_side_prepare));
#else
        retsv= boolSV(0);
#endif
      break;
    case 23:
      if (strEQ(key, "mysql_is_auto_increment"))
        retsv = ST_FETCH_AV(AV_ATTRIB_IS_AUTO_INCREMENT);
      break;
    case 37:
      if (strEQ(key, "mysql_server_prepare_disable_fallback"))
#if MYSQL_VERSION_ID >= SERVER_PREPARE_VERSION
        retsv= sv_2mortal(newSViv((IV) imp_sth->disable_fallback_for_server_prepare));
#else
        retsv= boolSV(0);
#endif
      break;
    }
    break;
  }
  return retsv;
}