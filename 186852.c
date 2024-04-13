int dbd_st_execute(SV* sth, imp_sth_t* imp_sth)
{
  dTHX;
  char actual_row_num[64];
  int i;
  SV **statement;
  D_imp_dbh_from_sth;
  D_imp_xxh(sth);
#if defined (dTHR)
  dTHR;
#endif

  ASYNC_CHECK_RETURN(sth, -2);

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
      " -> dbd_st_execute for %08lx\n", (u_long) sth);

  if (!SvROK(sth)  ||  SvTYPE(SvRV(sth)) != SVt_PVHV)
    croak("Expected hash array");

  /* Free cached array attributes */
  for (i= 0;  i < AV_ATTRIB_LAST;  i++)
  {
    if (imp_sth->av_attr[i])
      SvREFCNT_dec(imp_sth->av_attr[i]);

    imp_sth->av_attr[i]= Nullav;
  }

  statement= hv_fetch((HV*) SvRV(sth), "Statement", 9, FALSE);

  /* 
     Clean-up previous result set(s) for sth to prevent
     'Commands out of sync' error 
  */
  mysql_st_free_result_sets (sth, imp_sth);

#if MYSQL_VERSION_ID >= SERVER_PREPARE_VERSION

  if (imp_sth->use_server_side_prepare && ! imp_sth->use_mysql_use_result)
  {
    imp_sth->row_num= mysql_st_internal_execute41(
                                                  sth,
                                                  DBIc_NUM_PARAMS(imp_sth),
                                                  &imp_sth->result,
                                                  imp_sth->stmt,
                                                  imp_sth->bind,
                                                  &imp_sth->has_been_bound
                                                 );
  }
  else {
#endif
    imp_sth->row_num= mysql_st_internal_execute(
                                                sth,
                                                *statement,
                                                NULL,
                                                DBIc_NUM_PARAMS(imp_sth),
                                                imp_sth->params,
                                                &imp_sth->result,
                                                imp_dbh->pmysql,
                                                imp_sth->use_mysql_use_result
                                               );
#if MYSQL_ASYNC
    if(imp_dbh->async_query_in_flight) {
        DBIc_ACTIVE_on(imp_sth);
        return 0;
    }
#endif
  }

  if (imp_sth->row_num+1 != (my_ulonglong)-1)
  {
    if (!imp_sth->result)
    {
      imp_sth->insertid= mysql_insert_id(imp_dbh->pmysql);
#if MYSQL_VERSION_ID >= MULTIPLE_RESULT_SET_VERSION
      if (mysql_more_results(imp_dbh->pmysql))
        DBIc_ACTIVE_on(imp_sth);
#endif
    }
    else
    {
      /** Store the result in the current statement handle */
      DBIc_NUM_FIELDS(imp_sth)= mysql_num_fields(imp_sth->result);
      DBIc_ACTIVE_on(imp_sth);
      if (!imp_sth->use_server_side_prepare)
        imp_sth->done_desc= 0;
      imp_sth->fetch_done= 0;
    }
  }

  imp_sth->warning_count = mysql_warning_count(imp_dbh->pmysql);

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
  {
    /* 
      PerlIO_printf doesn't always handle imp_sth->row_num %llu 
      consistently!!
    */
    sprintf(actual_row_num, "%llu", imp_sth->row_num);
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                  " <- dbd_st_execute returning imp_sth->row_num %s\n",
                  actual_row_num);
  }

  return (int)imp_sth->row_num;
}