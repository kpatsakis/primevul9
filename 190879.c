   Reopen an IMAP stream to a new mailbox */
PHP_FUNCTION(imap_reopen)
{
	zval *streamind;
	zend_string *mailbox;
	zend_long options = 0, retries = 0;
	pils *imap_le_struct;
	long flags=NIL;
	long cl_flags=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS|ll", &streamind, &mailbox, &options, &retries) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (options) {
		flags = options;
		if (flags & PHP_EXPUNGE) {
			cl_flags = CL_EXPUNGE;
			flags ^= PHP_EXPUNGE;
		}
		imap_le_struct->flags = cl_flags;
	}
#ifdef SET_MAXLOGINTRIALS
	if (retries) {
		mail_parameters(NIL, SET_MAXLOGINTRIALS, (void *) retries);
	}
#endif
	/* local filename, need to perform open_basedir check */
	if (ZSTR_VAL(mailbox)[0] != '{' && php_check_open_basedir(ZSTR_VAL(mailbox))) {
		RETURN_FALSE;
	}

	imap_le_struct->imap_stream = mail_open(imap_le_struct->imap_stream, ZSTR_VAL(mailbox), flags);
	if (imap_le_struct->imap_stream == NIL) {
		zend_list_delete(Z_RES_P(streamind));
		php_error_docref(NULL, E_WARNING, "Couldn't re-open stream");
		RETURN_FALSE;
	}
	RETURN_TRUE;