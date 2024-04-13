my_ulonglong mysql_st_internal_execute41(
                                         SV *sth,
                                         int num_params,
                                         MYSQL_RES **result,
                                         MYSQL_STMT *stmt,
                                         MYSQL_BIND *bind,
                                         int *has_been_bound
                                        )
{
  int i;
  enum enum_field_types enum_type;
  dTHX;
  int execute_retval;
  my_ulonglong rows=0;
  D_imp_xxh(sth);

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                  "\t-> mysql_st_internal_execute41\n");

  /* free result if exists */
  if (*result)
  {
    mysql_free_result(*result);
    *result= 0;
  }

  /*
    If were performed any changes with ph variables
    we have to rebind them
  */

  if (num_params > 0 && !(*has_been_bound))
  {
    if (mysql_stmt_bind_param(stmt,bind))
      goto error;

    *has_been_bound= 1;
  }

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                  "\t\tmysql_st_internal_execute41 calling mysql_execute with %d num_params\n",
                  num_params);

  execute_retval= mysql_stmt_execute(stmt);
  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                  "\t\tmysql_stmt_execute returned %d\n",
                  execute_retval);
  if (execute_retval)
    goto error;

  /*
   This statement does not return a result set (INSERT, UPDATE...)
  */
  if (!(*result= mysql_stmt_result_metadata(stmt)))
  {
    if (mysql_stmt_errno(stmt))
      goto error;

    rows= mysql_stmt_affected_rows(stmt);

    /* mysql_stmt_affected_rows(): -1 indicates that the query returned an error */
    if (rows == (my_ulonglong)-1)
      goto error;
  }
  /*
    This statement returns a result set (SELECT...)
  */
  else
  {
    for (i = mysql_stmt_field_count(stmt) - 1; i >=0; --i) {
        enum_type = mysql_to_perl_type(stmt->fields[i].type);
        if (enum_type != MYSQL_TYPE_DOUBLE && enum_type != MYSQL_TYPE_LONG && enum_type != MYSQL_TYPE_LONGLONG && enum_type != MYSQL_TYPE_BIT)
        {
            /* mysql_stmt_store_result to update MYSQL_FIELD->max_length */
            my_bool on = 1;
            mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &on);
            break;
        }
    }
    /* Get the total rows affected and return */
    if (mysql_stmt_store_result(stmt))
      goto error;
    else
      rows= mysql_stmt_num_rows(stmt);
  }
  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                  "\t<- mysql_internal_execute_41 returning %llu rows\n",
                  rows);
  return(rows);

error:
  if (*result)
  {
    mysql_free_result(*result);
    *result= 0;
  }
  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                  "     errno %d err message %s\n",
                  mysql_stmt_errno(stmt),
                  mysql_stmt_error(stmt));
  do_error(sth, mysql_stmt_errno(stmt), mysql_stmt_error(stmt),
           mysql_stmt_sqlstate(stmt));
  mysql_stmt_reset(stmt);

  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                  "\t<- mysql_st_internal_execute41\n");
  return -2;

}