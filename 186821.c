int dbd_describe(SV* sth, imp_sth_t* imp_sth)
{
  dTHX;
  D_imp_xxh(sth);
  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\t--> dbd_describe\n");


#if MYSQL_VERSION_ID >= SERVER_PREPARE_VERSION

  if (imp_sth->use_server_side_prepare)
  {
    int i;
    int col_type;
    int num_fields= DBIc_NUM_FIELDS(imp_sth);
    imp_sth_fbh_t *fbh;
    MYSQL_BIND *buffer;
    MYSQL_FIELD *fields;

    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\t\tdbd_describe() num_fields %d\n",
                    num_fields);

    if (imp_sth->done_desc)
      return TRUE;

    if (!num_fields || !imp_sth->result)
    {
      /* no metadata */
      do_error(sth, JW_ERR_SEQUENCE,
               "no metadata information while trying describe result set",
               NULL);
      return 0;
    }

    /* allocate fields buffers  */
    if (  !(imp_sth->fbh= alloc_fbuffer(num_fields))
          || !(imp_sth->buffer= alloc_bind(num_fields)) )
    {
      /* Out of memory */
      do_error(sth, JW_ERR_SEQUENCE,
               "Out of memory in dbd_sescribe()",NULL);
      return 0;
    }

    fields= mysql_fetch_fields(imp_sth->result);

    for (
         fbh= imp_sth->fbh, buffer= (MYSQL_BIND*)imp_sth->buffer, i= 0;
         i < num_fields;
         i++, fbh++, buffer++
        )
    {
      /* get the column type */
      col_type = fields ? fields[i].type : MYSQL_TYPE_STRING;

      if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      {
        PerlIO_printf(DBIc_LOGPIO(imp_xxh),"\t\ti %d col_type %d fbh->length %lu\n",
                      i, col_type, fbh->length);
        PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                      "\t\tfields[i].length %lu fields[i].max_length %lu fields[i].type %d fields[i].charsetnr %d\n",
                      fields[i].length, fields[i].max_length, fields[i].type,
                      fields[i].charsetnr);
      }
      fbh->charsetnr = fields[i].charsetnr;
#if MYSQL_VERSION_ID < FIELD_CHARSETNR_VERSION 
      fbh->flags     = fields[i].flags;
#endif

      buffer->buffer_type= mysql_to_perl_type(col_type);
      if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
        PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\t\tmysql_to_perl_type returned %d\n",
                      col_type);
      buffer->length= &(fbh->length);
      buffer->is_null= (my_bool*) &(fbh->is_null);
      buffer->error= (my_bool*) &(fbh->error);

      switch (buffer->buffer_type) {
      case MYSQL_TYPE_DOUBLE:
        buffer->buffer_length= sizeof(fbh->ddata);
        buffer->buffer= (char*) &fbh->ddata;
        break;

      case MYSQL_TYPE_LONG:
      case MYSQL_TYPE_LONGLONG:
        buffer->buffer_length= sizeof(fbh->ldata);
        buffer->buffer= (char*) &fbh->ldata;
        buffer->is_unsigned= (fields[i].flags & UNSIGNED_FLAG) ? 1 : 0;
        break;

      case MYSQL_TYPE_BIT:
        buffer->buffer_length= 8;
        Newz(908, fbh->data, buffer->buffer_length, char);
        buffer->buffer= (char *) fbh->data;
        break;

      default:
        buffer->buffer_length= fields[i].max_length ? fields[i].max_length : 1;
        Newz(908, fbh->data, buffer->buffer_length, char);
        buffer->buffer= (char *) fbh->data;
      }
    }

    if (mysql_stmt_bind_result(imp_sth->stmt, imp_sth->buffer))
    {
      do_error(sth, mysql_stmt_errno(imp_sth->stmt),
               mysql_stmt_error(imp_sth->stmt),
               mysql_stmt_sqlstate(imp_sth->stmt));
      return 0;
    }
  }
#endif

  imp_sth->done_desc= 1;
  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "\t<- dbd_describe\n");
  return TRUE;
}