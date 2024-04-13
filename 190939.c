 */
static void php_imap_do_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	zend_string *mailbox, *user, *passwd;
	zend_long retries = 0, flags = NIL, cl_flags = NIL;
	MAILSTREAM *imap_stream;
	pils *imap_le_struct;
	zval *params = NULL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "PSS|lla", &mailbox, &user,
		&passwd, &flags, &retries, &params) == FAILURE) {
		return;
	}

	if (argc >= 4) {
		if (flags & PHP_EXPUNGE) {
			cl_flags = CL_EXPUNGE;
			flags ^= PHP_EXPUNGE;
		}
		if (flags & OP_PROTOTYPE) {
			cl_flags |= OP_PROTOTYPE;
		}
	}

	if (params) {
		zval *disabled_auth_method;

		if ((disabled_auth_method = zend_hash_str_find(Z_ARRVAL_P(params), "DISABLE_AUTHENTICATOR", sizeof("DISABLE_AUTHENTICATOR") - 1)) != NULL) {
			switch (Z_TYPE_P(disabled_auth_method)) {
				case IS_STRING:
					if (Z_STRLEN_P(disabled_auth_method) > 1) {
						mail_parameters(NIL, DISABLE_AUTHENTICATOR, (void *)Z_STRVAL_P(disabled_auth_method));
					}
					break;
				case IS_ARRAY:
					{
						zval *z_auth_method;
						int i;
						int nelems = zend_hash_num_elements(Z_ARRVAL_P(disabled_auth_method));

						if (nelems == 0 ) {
							break;
						}
						for (i = 0; i < nelems; i++) {
							if ((z_auth_method = zend_hash_index_find(Z_ARRVAL_P(disabled_auth_method), i)) != NULL) {
								if (Z_TYPE_P(z_auth_method) == IS_STRING) {
									if (Z_STRLEN_P(z_auth_method) > 1) {
										mail_parameters(NIL, DISABLE_AUTHENTICATOR, (void *)Z_STRVAL_P(z_auth_method));
									}
								} else {
									php_error_docref(NULL, E_WARNING, "Invalid argument, expect string or array of strings");
								}
							}
						}
					}
					break;
				case IS_LONG:
				default:
					php_error_docref(NULL, E_WARNING, "Invalid argument, expect string or array of strings");
					break;
			}
		}
	}

	if (IMAPG(imap_user)) {
		efree(IMAPG(imap_user));
		IMAPG(imap_user) = 0;
	}

	if (IMAPG(imap_password)) {
		efree(IMAPG(imap_password));
		IMAPG(imap_password) = 0;
	}

	/* local filename, need to perform open_basedir check */
	if (ZSTR_VAL(mailbox)[0] != '{' && php_check_open_basedir(ZSTR_VAL(mailbox))) {
		RETURN_FALSE;
	}

	IMAPG(imap_user)     = estrndup(ZSTR_VAL(user), ZSTR_LEN(user));
	IMAPG(imap_password) = estrndup(ZSTR_VAL(passwd), ZSTR_LEN(passwd));

#ifdef SET_MAXLOGINTRIALS
	if (argc >= 5) {
		if (retries < 0) {
			php_error_docref(NULL, E_WARNING ,"Retries must be greater or equal to 0");
		} else {
			mail_parameters(NIL, SET_MAXLOGINTRIALS, (void *) retries);
		}
	}
#endif

	imap_stream = mail_open(NIL, ZSTR_VAL(mailbox), flags);

	if (imap_stream == NIL) {
		php_error_docref(NULL, E_WARNING, "Couldn't open stream %s", ZSTR_VAL(mailbox));
		efree(IMAPG(imap_user)); IMAPG(imap_user) = 0;
		efree(IMAPG(imap_password)); IMAPG(imap_password) = 0;
		RETURN_FALSE;
	}

	imap_le_struct = emalloc(sizeof(pils));
	imap_le_struct->imap_stream = imap_stream;
	imap_le_struct->flags = cl_flags;

	RETURN_RES(zend_register_resource(imap_le_struct, le_imap));