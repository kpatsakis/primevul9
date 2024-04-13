MYSQL *mysql_dr_connect(
                        SV* dbh,
                        MYSQL* sock,
                        char* mysql_socket,
                        char* host,
			                  char* port,
                        char* user,
                        char* password,
			                  char* dbname,
                        imp_dbh_t *imp_dbh)
{
  int portNr;
  unsigned int client_flag;
  MYSQL* result;
  dTHX;
  D_imp_xxh(dbh);

  /* per Monty, already in client.c in API */
  /* but still not exist in libmysqld.c */
#if defined(DBD_MYSQL_EMBEDDED)
   if (host && !*host) host = NULL;
#endif

  portNr= (port && *port) ? atoi(port) : 0;

  /* already in client.c in API */
  /* if (user && !*user) user = NULL; */
  /* if (password && !*password) password = NULL; */


  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
		  "imp_dbh->mysql_dr_connect: host = |%s|, port = %d," \
		  " uid = %s, pwd = %s\n",
		  host ? host : "NULL", portNr,
		  user ? user : "NULL",
		  password ? password : "NULL");

  {

#if defined(DBD_MYSQL_EMBEDDED)
    if (imp_dbh)
    {
      D_imp_drh_from_dbh;
      SV* sv = DBIc_IMP_DATA(imp_dbh);

      if (sv  &&  SvROK(sv))
      {
        SV** svp;
        STRLEN lna;
        char * options;
        int server_args_cnt= 0;
        int server_groups_cnt= 0;
        int rc= 0;

        char ** server_args = NULL;
        char ** server_groups = NULL;

        HV* hv = (HV*) SvRV(sv);

        if (SvTYPE(hv) != SVt_PVHV)
          return NULL;

        if (!imp_drh->embedded.state)
        {
          /* Init embedded server */
          if ((svp = hv_fetch(hv, "mysql_embedded_groups", 21, FALSE))  &&
              *svp  &&  SvTRUE(*svp))
          {
            options = SvPV(*svp, lna);
            imp_drh->embedded.groups=newSVsv(*svp);

            if ((server_groups_cnt=count_embedded_options(options)))
            {
              /* number of server_groups always server_groups+1 */
              server_groups=fill_out_embedded_options(DBIc_LOGPIO(imp_xxh), options, 0, 
                                                      (int)lna, ++server_groups_cnt);
              if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
              {
                PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                              "Groups names passed to embedded server:\n");
                print_embedded_options(DBIc_LOGPIO(imp_xxh), server_groups, server_groups_cnt);
              }
            }
          }

          if ((svp = hv_fetch(hv, "mysql_embedded_options", 22, FALSE))  &&
              *svp  &&  SvTRUE(*svp))
          {
            options = SvPV(*svp, lna);
            imp_drh->embedded.args=newSVsv(*svp);

            if ((server_args_cnt=count_embedded_options(options)))
            {
              /* number of server_options always server_options+1 */
              server_args=fill_out_embedded_options(DBIc_LOGPIO(imp_xxh), options, 1, (int)lna, ++server_args_cnt);
              if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
              {
                PerlIO_printf(DBIc_LOGPIO(imp_xxh), "Server options passed to embedded server:\n");
                print_embedded_options(DBIc_LOGPIO(imp_xxh), server_args, server_args_cnt);
              }
            }
          }
          if (mysql_server_init(server_args_cnt, server_args, server_groups))
          {
            do_warn(dbh, AS_ERR_EMBEDDED, "Embedded server was not started. \
                    Could not initialize environment.");
            return NULL;
          }
          imp_drh->embedded.state=1;

          if (server_args_cnt)
            free_embedded_options(server_args, server_args_cnt);
          if (server_groups_cnt)
            free_embedded_options(server_groups, server_groups_cnt);
        }
        else
        {
         /*
          * Check if embedded parameters passed to connect() differ from
          * first ones
          */

          if ( ((svp = hv_fetch(hv, "mysql_embedded_groups", 21, FALSE)) &&
            *svp  &&  SvTRUE(*svp)))
            rc =+ abs(sv_cmp(*svp, imp_drh->embedded.groups));

          if ( ((svp = hv_fetch(hv, "mysql_embedded_options", 22, FALSE)) &&
            *svp  &&  SvTRUE(*svp)) )
            rc =+ abs(sv_cmp(*svp, imp_drh->embedded.args));

          if (rc)
          {
            do_warn(dbh, AS_ERR_EMBEDDED,
                    "Embedded server was already started. You cannot pass init\
                    parameters to embedded server once");
            return NULL;
          }
        }
      }
    }
#endif

#ifdef MYSQL_NO_CLIENT_FOUND_ROWS
    client_flag = 0;
#else
    client_flag = CLIENT_FOUND_ROWS;
#endif
    mysql_init(sock);

    if (imp_dbh)
    {
      SV* sv = DBIc_IMP_DATA(imp_dbh);

      DBIc_set(imp_dbh, DBIcf_AutoCommit, TRUE);
      if (sv  &&  SvROK(sv))
      {
        HV* hv = (HV*) SvRV(sv);
        SV** svp;
        STRLEN lna;

        /* thanks to Peter John Edwards for mysql_init_command */ 
        if ((svp = hv_fetch(hv, "mysql_init_command", 18, FALSE)) &&
            *svp && SvTRUE(*svp))
        {
          char* df = SvPV(*svp, lna);
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                           "imp_dbh->mysql_dr_connect: Setting" \
                           " init command (%s).\n", df);
          mysql_options(sock, MYSQL_INIT_COMMAND, df);
        }
        if ((svp = hv_fetch(hv, "mysql_compression", 17, FALSE))  &&
            *svp && SvTRUE(*svp))
        {
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                          "imp_dbh->mysql_dr_connect: Enabling" \
                          " compression.\n");
          mysql_options(sock, MYSQL_OPT_COMPRESS, NULL);
        }
        if ((svp = hv_fetch(hv, "mysql_connect_timeout", 21, FALSE))
            &&  *svp  &&  SvTRUE(*svp))
        {
          int to = SvIV(*svp);
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                          "imp_dbh->mysql_dr_connect: Setting" \
                          " connect timeout (%d).\n",to);
          mysql_options(sock, MYSQL_OPT_CONNECT_TIMEOUT,
                        (const char *)&to);
        }
        if ((svp = hv_fetch(hv, "mysql_write_timeout", 19, FALSE))
            &&  *svp  &&  SvTRUE(*svp))
        {
          int to = SvIV(*svp);
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                          "imp_dbh->mysql_dr_connect: Setting" \
                          " write timeout (%d).\n",to);
          mysql_options(sock, MYSQL_OPT_WRITE_TIMEOUT,
                        (const char *)&to);
        }
        if ((svp = hv_fetch(hv, "mysql_read_timeout", 18, FALSE))
            &&  *svp  &&  SvTRUE(*svp))
        {
          int to = SvIV(*svp);
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                          "imp_dbh->mysql_dr_connect: Setting" \
                          " read timeout (%d).\n",to);
          mysql_options(sock, MYSQL_OPT_READ_TIMEOUT,
                        (const char *)&to);
        }
        if ((svp = hv_fetch(hv, "mysql_skip_secure_auth", 22, FALSE)) &&
            *svp  &&  SvTRUE(*svp))
        {
          my_bool secauth = 0;
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                          "imp_dbh->mysql_dr_connect: Skipping" \
                          " secure auth\n");
          mysql_options(sock, MYSQL_SECURE_AUTH, &secauth);
        }
        if ((svp = hv_fetch(hv, "mysql_read_default_file", 23, FALSE)) &&
            *svp  &&  SvTRUE(*svp))
        {
          char* df = SvPV(*svp, lna);
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                          "imp_dbh->mysql_dr_connect: Reading" \
                          " default file %s.\n", df);
          mysql_options(sock, MYSQL_READ_DEFAULT_FILE, df);
        }
        if ((svp = hv_fetch(hv, "mysql_read_default_group", 24,
                            FALSE))  &&
            *svp  &&  SvTRUE(*svp)) {
          char* gr = SvPV(*svp, lna);
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                    "imp_dbh->mysql_dr_connect: Using" \
                    " default group %s.\n", gr);

          mysql_options(sock, MYSQL_READ_DEFAULT_GROUP, gr);
        }
        #if (MYSQL_VERSION_ID >= 50606)
          if ((svp = hv_fetch(hv, "mysql_conn_attrs", 16, FALSE)) && *svp) {
              HV* attrs = (HV*) SvRV(*svp);
              HE* entry = NULL;
              I32 num_entries = hv_iterinit(attrs);
              while (num_entries && (entry = hv_iternext(attrs))) {
                  I32 retlen = 0;
                  char *attr_name = hv_iterkey(entry, &retlen);
                  SV *sv_attr_val = hv_iterval(attrs, entry);
                  char *attr_val  = SvPV(sv_attr_val, lna);
                  mysql_options4(sock, MYSQL_OPT_CONNECT_ATTR_ADD, attr_name, attr_val);
              }
          }
        #endif
        if ((svp = hv_fetch(hv, "mysql_client_found_rows", 23, FALSE)) && *svp)
        {
          if (SvTRUE(*svp))
            client_flag |= CLIENT_FOUND_ROWS;
          else
            client_flag &= ~CLIENT_FOUND_ROWS;
        }
        if ((svp = hv_fetch(hv, "mysql_use_result", 16, FALSE)) && *svp)
        {
          imp_dbh->use_mysql_use_result = SvTRUE(*svp);
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                          "imp_dbh->use_mysql_use_result: %d\n",
                          imp_dbh->use_mysql_use_result);
        }
        if ((svp = hv_fetch(hv, "mysql_bind_type_guessing", 24, TRUE)) && *svp)
        {
          imp_dbh->bind_type_guessing= SvTRUE(*svp);
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                          "imp_dbh->bind_type_guessing: %d\n",
                          imp_dbh->bind_type_guessing);
        }
        if ((svp = hv_fetch(hv, "mysql_bind_comment_placeholders", 31, FALSE)) && *svp)
        {
          imp_dbh->bind_comment_placeholders = SvTRUE(*svp);
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                          "imp_dbh->bind_comment_placeholders: %d\n",
                          imp_dbh->bind_comment_placeholders);
        }
        if ((svp = hv_fetch(hv, "mysql_no_autocommit_cmd", 23, FALSE)) && *svp)
        {
          imp_dbh->no_autocommit_cmd= SvTRUE(*svp);
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                          "imp_dbh->no_autocommit_cmd: %d\n",
                          imp_dbh->no_autocommit_cmd);
        }
#if FABRIC_SUPPORT
        if ((svp = hv_fetch(hv, "mysql_use_fabric", 16, FALSE)) &&
            *svp && SvTRUE(*svp))
        {
          if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
            PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                          "imp_dbh->use_fabric: Enabling use of" \
                          " MySQL Fabric.\n");
          mysql_options(sock, MYSQL_OPT_USE_FABRIC, NULL);
        }
#endif

#if defined(CLIENT_MULTI_STATEMENTS)
	if ((svp = hv_fetch(hv, "mysql_multi_statements", 22, FALSE)) && *svp)
        {
	  if (SvTRUE(*svp))
	    client_flag |= CLIENT_MULTI_STATEMENTS;
          else
            client_flag &= ~CLIENT_MULTI_STATEMENTS;
	}
#endif

#if MYSQL_VERSION_ID >=SERVER_PREPARE_VERSION
	/* took out  client_flag |= CLIENT_PROTOCOL_41; */
	/* because libmysql.c already sets this no matter what */
	if ((svp = hv_fetch(hv, "mysql_server_prepare", 20, FALSE))
            && *svp)
        {
	  if (SvTRUE(*svp))
          {
	    client_flag |= CLIENT_PROTOCOL_41;
            imp_dbh->use_server_side_prepare = TRUE;
	  }
          else
          {
	    client_flag &= ~CLIENT_PROTOCOL_41;
            imp_dbh->use_server_side_prepare = FALSE;
	  }
	}
        if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
          PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                        "imp_dbh->use_server_side_prepare: %d\n",
                        imp_dbh->use_server_side_prepare);

        if ((svp = hv_fetch(hv, "mysql_server_prepare_disable_fallback", 37, FALSE)) && *svp)
          imp_dbh->disable_fallback_for_server_prepare = SvTRUE(*svp);
        if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
          PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                        "imp_dbh->disable_fallback_for_server_prepare: %d\n",
                        imp_dbh->disable_fallback_for_server_prepare);
#endif

        /* HELMUT */
#if defined(sv_utf8_decode) && MYSQL_VERSION_ID >=SERVER_PREPARE_VERSION
        if ((svp = hv_fetch(hv, "mysql_enable_utf8mb4", 20, FALSE)) && *svp && SvTRUE(*svp)) {
          mysql_options(sock, MYSQL_SET_CHARSET_NAME, "utf8mb4");
        }
        else if ((svp = hv_fetch(hv, "mysql_enable_utf8", 17, FALSE)) && *svp) {
          /* Do not touch imp_dbh->enable_utf8 as we are called earlier
           * than it is set and mysql_options() must be before:
           * mysql_real_connect()
          */
         mysql_options(sock, MYSQL_SET_CHARSET_NAME,
                       (SvTRUE(*svp) ? "utf8" : "latin1"));
         if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
           PerlIO_printf(DBIc_LOGPIO(imp_xxh),
                         "mysql_options: MYSQL_SET_CHARSET_NAME=%s\n",
                         (SvTRUE(*svp) ? "utf8" : "latin1"));
        }
#endif

#if defined(DBD_MYSQL_WITH_SSL) && !defined(DBD_MYSQL_EMBEDDED) && \
    (defined(CLIENT_SSL) || (MYSQL_VERSION_ID >= 40000))
	if ((svp = hv_fetch(hv, "mysql_ssl", 9, FALSE))  &&  *svp)
        {
	  if (SvTRUE(*svp))
          {
	    char *client_key = NULL;
	    char *client_cert = NULL;
	    char *ca_file = NULL;
	    char *ca_path = NULL;
	    char *cipher = NULL;
	    STRLEN lna;
#if MYSQL_VERSION_ID >= SSL_VERIFY_VERSION && MYSQL_VERSION_ID <= SSL_LAST_VERIFY_VERSION
            /*
              New code to utilise MySQLs new feature that verifies that the
              server's hostname that the client connects to matches that of
              the certificate
            */
	    my_bool ssl_verify_true = 0;
	    if ((svp = hv_fetch(hv, "mysql_ssl_verify_server_cert", 28, FALSE))  &&  *svp)
	      ssl_verify_true = SvTRUE(*svp);
#endif
	    if ((svp = hv_fetch(hv, "mysql_ssl_client_key", 20, FALSE)) && *svp)
	      client_key = SvPV(*svp, lna);

	    if ((svp = hv_fetch(hv, "mysql_ssl_client_cert", 21, FALSE)) &&
                *svp)
	      client_cert = SvPV(*svp, lna);

	    if ((svp = hv_fetch(hv, "mysql_ssl_ca_file", 17, FALSE)) &&
		 *svp)
	      ca_file = SvPV(*svp, lna);

	    if ((svp = hv_fetch(hv, "mysql_ssl_ca_path", 17, FALSE)) &&
                *svp)
	      ca_path = SvPV(*svp, lna);

	    if ((svp = hv_fetch(hv, "mysql_ssl_cipher", 16, FALSE)) &&
		*svp)
	      cipher = SvPV(*svp, lna);

	    mysql_ssl_set(sock, client_key, client_cert, ca_file,
			  ca_path, cipher);
#if MYSQL_VERSION_ID >= SSL_VERIFY_VERSION && MYSQL_VERSION_ID <= SSL_LAST_VERIFY_VERSION
	    mysql_options(sock, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &ssl_verify_true);
#endif
	    client_flag |= CLIENT_SSL;
	  }
	}
#endif
#if (MYSQL_VERSION_ID >= 32349)
	/*
	 * MySQL 3.23.49 disables LOAD DATA LOCAL by default. Use
	 * mysql_local_infile=1 in the DSN to enable it.
	 */
     if ((svp = hv_fetch( hv, "mysql_local_infile", 18, FALSE))  &&  *svp)
     {
	  unsigned int flag = SvTRUE(*svp);
    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
	    PerlIO_printf(DBIc_LOGPIO(imp_xxh),
        "imp_dbh->mysql_dr_connect: Using" \
        " local infile %u.\n", flag);
	  mysql_options(sock, MYSQL_OPT_LOCAL_INFILE, (const char *) &flag);
	}
#endif
      }
    }
    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      PerlIO_printf(DBIc_LOGPIO(imp_xxh), "imp_dbh->mysql_dr_connect: client_flags = %d\n",
		    client_flag);

#if MYSQL_VERSION_ID >= MULTIPLE_RESULT_SET_VERSION
    client_flag|= CLIENT_MULTI_RESULTS;
#endif
    result = mysql_real_connect(sock, host, user, password, dbname,
				portNr, mysql_socket, client_flag);
    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      PerlIO_printf(DBIc_LOGPIO(imp_xxh), "imp_dbh->mysql_dr_connect: <-");

    if (result)
    {
#if MYSQL_VERSION_ID >=SERVER_PREPARE_VERSION
      /* connection succeeded. */
      /* imp_dbh == NULL when mysql_dr_connect() is called from mysql.xs
         functions (_admin_internal(),_ListDBs()). */
      if (!(result->client_flag & CLIENT_PROTOCOL_41) && imp_dbh)
        imp_dbh->use_server_side_prepare = FALSE;
#endif

#if MYSQL_ASYNC
      if(imp_dbh) {
          imp_dbh->async_query_in_flight = NULL;
      }
#endif

      /*
        we turn off Mysql's auto reconnect and handle re-connecting ourselves
        so that we can keep track of when this happens.
      */
      result->reconnect=0;
    }
    else {
      /* 
         sock was allocated with mysql_init() 
         fixes: https://rt.cpan.org/Ticket/Display.html?id=86153

      Safefree(sock);

         rurban: No, we still need this handle later in mysql_dr_error().
         RT #97625. It will be freed as imp_dbh->pmysql in dbd_db_destroy(),
         which is called by the DESTROY handler.
      */
    }
    return result;
  }
}