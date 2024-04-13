my_ulonglong mysql_st_internal_execute(
                                       SV *h, /* could be sth or dbh */
                                       SV *statement,
                                       SV *attribs,
                                       int num_params,
                                       imp_sth_ph_t *params,
                                       MYSQL_RES **result,
                                       MYSQL *svsock,
                                       int use_mysql_use_result
                                      )
{
  dTHX;
  bool bind_type_guessing= FALSE;
  bool bind_comment_placeholders= TRUE;
  STRLEN slen;
  char *sbuf = SvPV(statement, slen);
  char *table;
  char *salloc;
  int htype;
  int errno;
#if MYSQL_ASYNC
  bool async = FALSE;
#endif
  my_ulonglong rows= 0;
  /* thank you DBI.c for this info! */
  D_imp_xxh(h);
  attribs= attribs;

  htype= DBIc_TYPE(imp_xxh);
  /*
    It is important to import imp_dbh properly according to the htype
    that it is! Also, one might ask why bind_type_guessing is assigned
    in each block. Well, it's because D_imp_ macros called in these
    blocks make it so imp_dbh is not "visible" or defined outside of the
    if/else (when compiled, it fails for imp_dbh not being defined).
  */
  /* h is a dbh */
  if (htype == DBIt_DB)
  {
    D_imp_dbh(h);
    /* if imp_dbh is not available, it causes segfault (proper) on OpenBSD */
    if (imp_dbh && imp_dbh->bind_type_guessing)
    {
      bind_type_guessing= imp_dbh->bind_type_guessing;
      bind_comment_placeholders= bind_comment_placeholders;
    }
#if MYSQL_ASYNC
    async = (bool) (imp_dbh->async_query_in_flight != NULL);
#endif
  }
  /* h is a sth */
  else
  {
    D_imp_sth(h);
    D_imp_dbh_from_sth;
    /* if imp_dbh is not available, it causes segfault (proper) on OpenBSD */
    if (imp_dbh)
    {
      bind_type_guessing= imp_dbh->bind_type_guessing;
      bind_comment_placeholders= imp_dbh->bind_comment_placeholders;
    }
#if MYSQL_ASYNC
    async = imp_sth->is_async;
    if(async) {
        imp_dbh->async_query_in_flight = imp_sth;
    } else {
        imp_dbh->async_query_in_flight = NULL;
    }
#endif
  }

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "mysql_st_internal_execute MYSQL_VERSION_ID %d\n",
                  MYSQL_VERSION_ID );

  salloc= parse_params(imp_xxh,
                              aTHX_ svsock,
                              sbuf,
                              &slen,
                              params,
                              num_params,
                              bind_type_guessing,
                              bind_comment_placeholders);

  if (salloc)
  {
    sbuf= salloc;
    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      PerlIO_printf(DBIc_LOGPIO(imp_xxh), "Binding parameters: %s\n", sbuf);
  }

  if (slen >= 11 && (!strncmp(sbuf, "listfields ", 11) ||
                     !strncmp(sbuf, "LISTFIELDS ", 11)))
  {
    /* remove pre-space */
    slen-= 10;
    sbuf+= 10;
    while (slen && isspace(*sbuf)) { --slen;  ++sbuf; }

    if (!slen)
    {
      do_error(h, JW_ERR_QUERY, "Missing table name" ,NULL);
      return -2;
    }
    if (!(table= malloc(slen+1)))
    {
      do_error(h, JW_ERR_MEM, "Out of memory" ,NULL);
      return -2;
    }

    strncpy(table, sbuf, slen);
    sbuf= table;

    while (slen && !isspace(*sbuf))
    {
      --slen;
      ++sbuf;
    }
    *sbuf++= '\0';

    *result= mysql_list_fields(svsock, table, NULL);

    free(table);

    if (!(*result))
    {
      do_error(h, mysql_errno(svsock), mysql_error(svsock)
               ,mysql_sqlstate(svsock));
      return -2;
    }

    return 0;
  }

#if MYSQL_ASYNC
  if(async) {
    if((mysql_send_query(svsock, sbuf, slen)) &&
       (!mysql_db_reconnect(h) ||
        (mysql_send_query(svsock, sbuf, slen))))
    {
        rows = -2;
    } else {
        rows = 0;
    }
  } else {
#endif
      if ((mysql_real_query(svsock, sbuf, slen))  &&
          (!mysql_db_reconnect(h)  ||
           (mysql_real_query(svsock, sbuf, slen))))
      {
        rows = -2;
      } else {
          /** Store the result from the Query */
          *result= use_mysql_use_result ?
            mysql_use_result(svsock) : mysql_store_result(svsock);

          if (mysql_errno(svsock))
            rows = -2;
          else if (*result)
            rows = mysql_num_rows(*result);
          else {
            rows = mysql_affected_rows(svsock);
            /* mysql_affected_rows(): -1 indicates that the query returned an error */
            if (rows == (my_ulonglong)-1)
              rows = -2;
          }
      }
#if MYSQL_ASYNC
  }
#endif

  if (salloc)
    Safefree(salloc);

  if(rows == -2) {
    do_error(h, mysql_errno(svsock), mysql_error(svsock), 
             mysql_sqlstate(svsock));
    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      PerlIO_printf(DBIc_LOGPIO(imp_xxh), "IGNORING ERROR errno %d\n", errno);
  }
  return(rows);
}