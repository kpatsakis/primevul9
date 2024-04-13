 */
static void php_imap_do_open(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char *mailbox, *user, *passwd;
	int mailbox_len, user_len, passwd_len;
	long retries = 0, flags = NIL, cl_flags = NIL;
	MAILSTREAM *imap_stream;
	pils *imap_le_struct;
	zval *params = NULL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "pss|lla", &mailbox, &mailbox_len, &user, &user_len,
		&passwd, &passwd_len, &flags, &retries, &params) == FAILURE) {
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
		zval **disabled_auth_method;

		if (zend_hash_find(HASH_OF(params), "DISABLE_AUTHENTICATOR", sizeof("DISABLE_AUTHENTICATOR"), (void **)&disabled_auth_method) == SUCCESS) {
			switch (Z_TYPE_PP(disabled_auth_method)) {
				case IS_STRING:
					if (Z_STRLEN_PP(disabled_auth_method) > 1) {
						mail_parameters (NIL, DISABLE_AUTHENTICATOR, (void *)Z_STRVAL_PP(disabled_auth_method));
					}
					break;
				case IS_ARRAY:
					{
						zval **z_auth_method;
						int i;
						int nelems = zend_hash_num_elements(Z_ARRVAL_PP(disabled_auth_method));

						if (nelems == 0 ) {
							break;
						}
						for (i = 0; i < nelems; i++) {
							if (zend_hash_index_find(Z_ARRVAL_PP(disabled_auth_method), i, (void **) &z_auth_method) == SUCCESS) {
								if (Z_TYPE_PP(z_auth_method) == IS_STRING) {
									if (Z_STRLEN_PP(z_auth_method) > 1) {
										mail_parameters (NIL, DISABLE_AUTHENTICATOR, (void *)Z_STRVAL_PP(z_auth_method));
									}
								} else {
									php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid argument, expect string or array of strings");
								}
							}
						}
					}
					break;
				case IS_LONG:
				default:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid argument, expect string or array of strings");
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
	if (mailbox[0] != '{' && php_check_open_basedir(mailbox TSRMLS_CC)) {
		RETURN_FALSE;
	}

	IMAPG(imap_user)     = estrndup(user, user_len);
	IMAPG(imap_password) = estrndup(passwd, passwd_len);

#ifdef SET_MAXLOGINTRIALS
	if (argc >= 5) {
		if (retries < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING ,"Retries must be greater or equal to 0");
		} else {
			mail_parameters(NIL, SET_MAXLOGINTRIALS, (void *) retries);
		}
	}
#endif

	imap_stream = mail_open(NIL, mailbox, flags);

	if (imap_stream == NIL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't open stream %s", mailbox);
		efree(IMAPG(imap_user)); IMAPG(imap_user) = 0;
		efree(IMAPG(imap_password)); IMAPG(imap_password) = 0;
		RETURN_FALSE;
	}

	imap_le_struct = emalloc(sizeof(pils));
	imap_le_struct->imap_stream = imap_stream;
	imap_le_struct->flags = cl_flags;

	ZEND_REGISTER_RESOURCE(return_value, imap_le_struct, le_imap);