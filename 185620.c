   Reopen an IMAP stream to a new mailbox */
PHP_FUNCTION(imap_reopen)
{
	zval *streamind;
	char *mailbox;
	int mailbox_len;
	long options = 0, retries = 0;
	pils *imap_le_struct;
	long flags=NIL;
	long cl_flags=NIL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|ll", &streamind, &mailbox, &mailbox_len, &options, &retries) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, &streamind, -1, "imap", le_imap);

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
	if (mailbox[0] != '{' && php_check_open_basedir(mailbox TSRMLS_CC)) {
		RETURN_FALSE;
	}

	imap_le_struct->imap_stream = mail_open(imap_le_struct->imap_stream, mailbox, flags);
	if (imap_le_struct->imap_stream == NIL) {
		zend_list_delete(Z_RESVAL_P(streamind));
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't re-open stream");
		RETURN_FALSE;
	}
	RETURN_TRUE;