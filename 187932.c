static int replace_user_table(THD *thd, TABLE *table, LEX_USER *combo,
			      ulong rights, bool revoke_grant,
			      bool can_create_user, bool no_auto_create)
{
  int error = -1;
  bool old_row_exists=0;
  char *password= empty_c_string;
  uint password_len= 0;
  char what= (revoke_grant) ? 'N' : 'Y';
  uchar user_key[MAX_KEY_LENGTH];
  LEX *lex= thd->lex;
  Acl_table_intact table_intact;
  DBUG_ENTER("replace_user_table");

  mysql_mutex_assert_owner(&acl_cache->lock);

  if (table_intact.check(table, &mysql_user_table_def))
    goto end;

  if (!table->key_info)
  {
    my_error(ER_TABLE_CORRUPT, MYF(0), table->s->db.str,
             table->s->table_name.str);
    goto end;
  }
 
  table->use_all_columns();
  DBUG_ASSERT(combo->host.str != '\0');
  table->field[MYSQL_USER_FIELD_HOST]->store(combo->host.str,combo->host.length,
                                             system_charset_info);
  table->field[MYSQL_USER_FIELD_USER]->store(combo->user.str,combo->user.length,
                                             system_charset_info);
  key_copy(user_key, table->record[0], table->key_info,
           table->key_info->key_length);

  if (table->file->ha_index_read_idx_map(table->record[0], 0, user_key,
                                         HA_WHOLE_KEY,
                                         HA_READ_KEY_EXACT))
  {
    /*
      The user record wasn't found; if the intention was to revoke privileges
      (indicated by what == 'N') then execution must fail now.
    */
    if (what == 'N')
    {
      my_error(ER_NONEXISTING_GRANT, MYF(0), combo->user.str, combo->host.str);
      goto end;
    }

    if ((!combo->uses_identified_by_clause &&
         !combo->uses_identified_with_clause &&
         !combo->uses_identified_by_password_clause) ||
        (combo->uses_identified_with_clause &&
         (!my_strcasecmp(system_charset_info, combo->plugin.str,
                         native_password_plugin_name.str) ||
          !my_strcasecmp(system_charset_info, combo->plugin.str,
                         old_password_plugin_name.str))))
    {
      if (check_password_policy(NULL))
      {
        error= 1;
        goto end;
      }
    }
    /* 1. Unresolved plugins become default plugin */
    if (!combo->uses_identified_with_clause)
    {
      combo->plugin.str= default_auth_plugin_name.str;
      combo->plugin.length= default_auth_plugin_name.length;
    }
    /* 2. Digest password if needed (plugin must have been resolved) */
    if (combo->uses_identified_by_clause)
    {
      if (digest_password(thd, combo))
      {
        my_error(ER_OUTOFMEMORY, MYF(ME_FATALERROR), CRYPT_MAX_PASSWORD_SIZE);
        error= 1;
        goto end;
      }
    }
    password= combo->password.str;
    password_len= combo->password.length;
    /*
      There are four options which affect the process of creation of
      a new user (mysqld option --safe-create-user, 'insert' privilege
      on 'mysql.user' table, using 'GRANT' with 'IDENTIFIED BY' and
      SQL_MODE flag NO_AUTO_CREATE_USER). Below is the simplified rule
      how it should work.
      if (safe-user-create && ! INSERT_priv) => reject
      else if (identified_by) => create
      else if (no_auto_create_user) => reject
      else create

      see also test_if_create_new_users()
    */
    if (!password_len &&
        auth_plugin_is_built_in(combo->plugin.str) && 
        no_auto_create)
    {
      my_error(ER_PASSWORD_NO_MATCH, MYF(0), combo->user.str, combo->host.str);
      goto end;
    }
    else if (!can_create_user)
    {
      my_error(ER_CANT_CREATE_USER_WITH_GRANT, MYF(0));
      goto end;
    }
    else if (combo->plugin.str[0])
    {
      if (!plugin_is_ready(&combo->plugin, MYSQL_AUTHENTICATION_PLUGIN))
      {
        my_error(ER_PLUGIN_IS_NOT_LOADED, MYF(0), combo->plugin.str);
        goto end;
      }
    }

    old_row_exists = 0;
    restore_record(table,s->default_values);
    DBUG_ASSERT(combo->host.str != '\0');
    table->field[MYSQL_USER_FIELD_HOST]->store(combo->host.str,combo->host.length,
                                               system_charset_info);
    table->field[MYSQL_USER_FIELD_USER]->store(combo->user.str,combo->user.length,
                                               system_charset_info);
#if defined(HAVE_OPENSSL)
    if (combo->plugin.str == sha256_password_plugin_name.str)
    {
      /* Use the authentication_string field */
      combo->auth.str= password;
      combo->auth.length= password_len;
      if (table->s->fields >= MYSQL_USER_FIELD_AUTHENTICATION_STRING)
      {
        if (password_len > 0)
          table->
            field[MYSQL_USER_FIELD_AUTHENTICATION_STRING]->
              store(password, password_len, &my_charset_utf8_bin);
        /* Assert that the proper plugin is set */
        table->
          field[MYSQL_USER_FIELD_PLUGIN]->
            store(sha256_password_plugin_name.str,
                  sha256_password_plugin_name.length,
                  system_charset_info);
      }
      else
      {
        my_error(ER_BAD_FIELD_ERROR, MYF(0), "plugin", "mysql.user");
        goto end;
      }
    }
    else
#endif
    {
      /* Use the legacy Password field */
      table->field[MYSQL_USER_FIELD_PASSWORD]->store(password, password_len,
                                                     system_charset_info);
      if (table->s->fields >= MYSQL_USER_FIELD_AUTHENTICATION_STRING)
        table->field[MYSQL_USER_FIELD_AUTHENTICATION_STRING]->store("\0", 0,
                                                       &my_charset_utf8_bin);
    }
  }
  else // if (table->file->ha_index_read_idx_map [..]
  {
    /*
      There is a matching user record ------------------------------------------
     */

    old_row_exists = 1;
    store_record(table,record[1]);			// Save copy for update

    /*
      GRANT statement using IDENTIFIED WITH clause can be used only to create
      user and apply privileges to it. Hence it throws an error when used on
      existing users.
    */
    if (combo->uses_identified_with_clause)
    {
      error= 1;
      my_error(ER_GRANT_PLUGIN_USER_EXISTS, MYF(0), combo->user.length,
	       combo->user.str);
      goto end;
    }
    
    /* 1. resolve plugins in the LEX_USER struct if needed */
    LEX_STRING old_plugin;

    /*
      Get old plugin value from storage.
    */

    old_plugin.str=
      get_field(thd->mem_root, table->field[MYSQL_USER_FIELD_PLUGIN]);

    /* 
      It is important not to include the trailing '\0' in the string length 
      because otherwise the plugin hash search will fail.
    */
    if (old_plugin.str)
    {
      old_plugin.length= strlen(old_plugin.str);

      /*
        Optimize for pointer comparision of built-in plugin name
      */

      optimize_plugin_compare_by_pointer(&old_plugin);

      /*
        Disable plugin change for existing rows with anything but
        the built in plugins.
        The idea is that all built in plugins support
        IDENTIFIED BY ... and none of the external ones currently do.
      */
      if ((combo->uses_identified_by_clause ||
	   combo->uses_identified_by_password_clause) &&
	  !auth_plugin_is_built_in(old_plugin.str))
      {
	const char *new_plugin= (combo->plugin.str && combo->plugin.str[0]) ?
	  combo->plugin.str : default_auth_plugin_name.str;

	if (my_strcasecmp(system_charset_info, new_plugin, old_plugin.str))
	{
	  push_warning(thd, Sql_condition::WARN_LEVEL_WARN, 
	    ER_SET_PASSWORD_AUTH_PLUGIN, ER(ER_SET_PASSWORD_AUTH_PLUGIN));
	}
      }
    }
    else
      old_plugin.length= 0;
    combo->plugin= old_plugin;

    /*
      If the plugin value in user table is found to be null or an empty
      string, the following steps are followed:

      * If GRANT is used with IDENTIFIED BY PASSWORD clause, and the hash
        is found to be of mysql_native_password or mysql_old_password
        type, the statement passes without an error and the password field
        is updated accordingly.
      * If GRANT is used with IDENTIFIED BY clause and the password is
        provided as a plain string, hashing of the string is done according
        to the value of old_passwords variable in the following way.

         if old_passwords == 0, mysql_native hashing is used.
	 if old_passwords == 1, mysql_old hashing is used.
	 if old_passwords == 2, error.
      * An empty password is considered to be of mysql_native type.
    */
    
    if (combo->plugin.str == NULL || combo->plugin.str == '\0')
    {
      if (combo->uses_identified_by_password_clause)
      {
	if ((combo->password.length == SCRAMBLED_PASSWORD_CHAR_LENGTH) ||
	    (combo->password.length == 0))
	{
	  combo->plugin.str= native_password_plugin_name.str;
	  combo->plugin.length= native_password_plugin_name.length;
	}
	else if (combo->password.length == SCRAMBLED_PASSWORD_CHAR_LENGTH_323)
	{
	  combo->plugin.str= old_password_plugin_name.str;
	  combo->plugin.length= old_password_plugin_name.length;
	}
	else
	{
	  /*
	    If hash length doesn't match either with mysql_native hash length or
	    mysql_old hash length, throw an error.
	  */
	  my_error(ER_PASSWORD_FORMAT, MYF(0));
	  error= 1;
	  goto end;
	}
      }
      else
      {
	/*
	  Handling of combo->plugin when IDENTIFIED BY PASSWORD clause is not
	  used, i.e. when the password hash is not provided within the GRANT
	  query.
	*/
	if ((thd->variables.old_passwords == 1) && (combo->password.length != 0))
	{
	  combo->plugin.str= old_password_plugin_name.str;
	  combo->plugin.length= old_password_plugin_name.length;
	}
	else if ((thd->variables.old_passwords == 0) || 
		 (combo->password.length == 0))
	{
	  combo->plugin.str= native_password_plugin_name.str;
	  combo->plugin.length= native_password_plugin_name.length;
	}
	else
	{
	  /* If old_passwords variable is neither 0 nor 1, throw an error. */
	  my_error(ER_PASSWORD_FORMAT, MYF(0));
	  error= 1;
	  goto end;
	}
      }
    }
    
    if (!combo->uses_authentication_string_clause)
    {
      combo->auth.str= get_field(thd->mem_root,
        table->field[MYSQL_USER_FIELD_AUTHENTICATION_STRING]);
      if (combo->auth.str)
        combo->auth.length= strlen(combo->auth.str);
      else
        combo->auth.length= 0;
    }
    /* 2. Digest password if needed (plugin must have been resolved */
    if (combo->uses_identified_by_clause)
    {
      if (digest_password(thd, combo))
      {
        error= 1;
        goto end;
      }
    }
    password= combo->password.str;
    password_len= combo->password.length;

    if (password_len > 0)
    {
#if defined(HAVE_OPENSSL)
      if (combo->plugin.str == sha256_password_plugin_name.str)
      {
        table->field[MYSQL_USER_FIELD_AUTHENTICATION_STRING]->
          store(password, password_len, &my_charset_utf8_bin);
        combo->auth.str= password;
        combo->auth.length= password_len;
      }
      else
#endif
      {
        table->field[MYSQL_USER_FIELD_PASSWORD]->
          store(password, password_len, system_charset_info);
        table->field[MYSQL_USER_FIELD_AUTHENTICATION_STRING]->
          store("\0", 0, &my_charset_utf8_bin);
      }
    }
    else if (!rights && !revoke_grant &&
             lex->ssl_type == SSL_TYPE_NOT_SPECIFIED &&
             !lex->mqh.specified_limits)
    {
     
      DBUG_PRINT("info", ("Proxy user exit path"));
      DBUG_RETURN(0);
    }
  }

  /* error checks on password */
  if (password_len > 0)
  {
    /*
     We need to check for hash validity here since later, when
     set_user_salt() is executed it will be too late to signal
     an error.
    */
    if ((combo->plugin.str == native_password_plugin_name.str &&
         password_len != SCRAMBLED_PASSWORD_CHAR_LENGTH) ||
        (combo->plugin.str == old_password_plugin_name.str &&
         password_len != SCRAMBLED_PASSWORD_CHAR_LENGTH_323))
    {
      my_error(ER_PASSWORD_FORMAT, MYF(0));
      error= 1;
      goto end;
    }
    /* The legacy Password field is used */
    if (combo->plugin.str == old_password_plugin_name.str)
      WARN_DEPRECATED_41_PWD_HASH(thd);
  }

  /* Update table columns with new privileges */

  Field **tmp_field;
  ulong priv;
  uint next_field;
  for (tmp_field= table->field+3, priv = SELECT_ACL;
       *tmp_field && (*tmp_field)->real_type() == MYSQL_TYPE_ENUM &&
	 ((Field_enum*) (*tmp_field))->typelib->count == 2 ;
       tmp_field++, priv <<= 1)
  {
    if (priv & rights)				 // set requested privileges
      (*tmp_field)->store(&what, 1, &my_charset_latin1);
  }
  rights= get_access(table, 3, &next_field);
  DBUG_PRINT("info",("table fields: %d",table->s->fields));
  if (table->s->fields >= 31)		/* From 4.0.0 we have more fields */
  {
    /* We write down SSL related ACL stuff */
    switch (lex->ssl_type) {
    case SSL_TYPE_ANY:
      table->field[MYSQL_USER_FIELD_SSL_TYPE]->store(STRING_WITH_LEN("ANY"),
                                      &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_SSL_CIPHER]->
        store("", 0, &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_X509_ISSUER]->store("", 0, &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_X509_SUBJECT]->store("", 0, &my_charset_latin1);
      break;
    case SSL_TYPE_X509:
      table->field[MYSQL_USER_FIELD_SSL_TYPE]->store(STRING_WITH_LEN("X509"),
                                      &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_SSL_CIPHER]->
        store("", 0, &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_X509_ISSUER]->store("", 0, &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_X509_SUBJECT]->store("", 0, &my_charset_latin1);
      break;
    case SSL_TYPE_SPECIFIED:
      table->field[MYSQL_USER_FIELD_SSL_TYPE]->store(STRING_WITH_LEN("SPECIFIED"),
                                      &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_SSL_CIPHER]->store("", 0, &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_X509_ISSUER]->store("", 0, &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_X509_SUBJECT]->store("", 0, &my_charset_latin1);
      if (lex->ssl_cipher)
        table->field[MYSQL_USER_FIELD_SSL_CIPHER]->store(lex->ssl_cipher,
                                strlen(lex->ssl_cipher), system_charset_info);
      if (lex->x509_issuer)
        table->field[MYSQL_USER_FIELD_X509_ISSUER]->store(lex->x509_issuer,
                                strlen(lex->x509_issuer), system_charset_info);
      if (lex->x509_subject)
        table->field[MYSQL_USER_FIELD_X509_SUBJECT]->store(lex->x509_subject,
                                strlen(lex->x509_subject), system_charset_info);
      break;
    case SSL_TYPE_NOT_SPECIFIED:
      break;
    case SSL_TYPE_NONE:
      table->field[MYSQL_USER_FIELD_SSL_TYPE]->store("", 0, &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_SSL_CIPHER]->store("", 0, &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_X509_ISSUER]->store("", 0, &my_charset_latin1);
      table->field[MYSQL_USER_FIELD_X509_SUBJECT]->store("", 0, &my_charset_latin1);
      break;
    }
    next_field+=4;

    USER_RESOURCES mqh= lex->mqh;
    if (mqh.specified_limits & USER_RESOURCES::QUERIES_PER_HOUR)
      table->field[MYSQL_USER_FIELD_MAX_QUESTIONS]->
        store((longlong) mqh.questions, TRUE);
    if (mqh.specified_limits & USER_RESOURCES::UPDATES_PER_HOUR)
      table->field[MYSQL_USER_FIELD_MAX_UPDATES]->
        store((longlong) mqh.updates, TRUE);
    if (mqh.specified_limits & USER_RESOURCES::CONNECTIONS_PER_HOUR)
      table->field[MYSQL_USER_FIELD_MAX_CONNECTIONS]->
        store((longlong) mqh.conn_per_hour, TRUE);
    if (table->s->fields >= 36 &&
        (mqh.specified_limits & USER_RESOURCES::USER_CONNECTIONS))
      table->field[MYSQL_USER_FIELD_MAX_USER_CONNECTIONS]->
        store((longlong) mqh.user_conn, TRUE);
    mqh_used= mqh_used || mqh.questions || mqh.updates || mqh.conn_per_hour;

    next_field+= 4;
    if (combo->plugin.length > 0 && !old_row_exists)
    {
      if (table->s->fields >= 41)
      {
        table->field[MYSQL_USER_FIELD_PLUGIN]->
          store(combo->plugin.str, combo->plugin.length, system_charset_info);
        table->field[MYSQL_USER_FIELD_PLUGIN]->set_notnull();
        table->field[MYSQL_USER_FIELD_AUTHENTICATION_STRING]->
          store(combo->auth.str, combo->auth.length, &my_charset_utf8_bin);
        table->field[MYSQL_USER_FIELD_AUTHENTICATION_STRING]->set_notnull();
      }
      else
      {
        my_error(ER_BAD_FIELD_ERROR, MYF(0), "plugin", "mysql.user");
        goto end;
      }
    }

    /* if we have a password supplied we update the expiration field */
    if (table->s->fields > MYSQL_USER_FIELD_PASSWORD_EXPIRED &&
        password_len > 0)
      table->field[MYSQL_USER_FIELD_PASSWORD_EXPIRED]->store("N", 1,
                                                             system_charset_info);
  }

  if (old_row_exists)
  {   
    /*
      We should NEVER delete from the user table, as a uses can still
      use mysqld even if he doesn't have any privileges in the user table!
    */
    if (cmp_record(table,record[1]))
    {
      if ((error=
           table->file->ha_update_row(table->record[1],table->record[0])) &&
          error != HA_ERR_RECORD_IS_THE_SAME)
      {						// This should never happen
        table->file->print_error(error,MYF(0));	/* purecov: deadcode */
        error= -1;				/* purecov: deadcode */
        goto end;				/* purecov: deadcode */
      }
      else
        error= 0;
    }
  }
  else if ((error=table->file->ha_write_row(table->record[0]))) // insert
  {						// This should never happen
    if (table->file->is_fatal_error(error, HA_CHECK_DUP))
    {
      table->file->print_error(error,MYF(0));	/* purecov: deadcode */
      error= -1;				/* purecov: deadcode */
      goto end;					/* purecov: deadcode */
    }
  }
  error=0;					// Privileges granted / revoked

end:
  if (!error)
  {
    acl_cache->clear(1);			// Clear privilege cache
    if (old_row_exists)
      acl_update_user(combo->user.str, combo->host.str,
                      combo->password.str, password_len,
		      lex->ssl_type,
		      lex->ssl_cipher,
		      lex->x509_issuer,
		      lex->x509_subject,
		      &lex->mqh,
		      rights,
		      &combo->plugin,
		      &combo->auth);
    else
      acl_insert_user(combo->user.str, combo->host.str, password, password_len,
		      lex->ssl_type,
		      lex->ssl_cipher,
		      lex->x509_issuer,
		      lex->x509_subject,
		      &lex->mqh,
		      rights,
		      &combo->plugin,
		      &combo->auth);
  }
  DBUG_RETURN(error);
}