   Send an email message */
PHP_FUNCTION(imap_mail)
{
	zend_string *to=NULL, *message=NULL, *headers=NULL, *subject=NULL, *cc=NULL, *bcc=NULL, *rpath=NULL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "SSS|SSSS", &to, &subject, &message,
		&headers, &cc, &bcc, &rpath) == FAILURE) {
		return;
	}

	/* To: */
	if (!ZSTR_LEN(to)) {
		php_error_docref(NULL, E_WARNING, "No to field in mail command");
		RETURN_FALSE;
	}

	/* Subject: */
	if (!ZSTR_LEN(subject)) {
		php_error_docref(NULL, E_WARNING, "No subject field in mail command");
		RETURN_FALSE;
	}

	/* message body */
	if (!ZSTR_LEN(message)) {
		/* this is not really an error, so it is allowed. */
		php_error_docref(NULL, E_WARNING, "No message string in mail command");
		message = NULL;
	}

	if (_php_imap_mail(ZSTR_VAL(to), ZSTR_VAL(subject), ZSTR_VAL(message), headers?ZSTR_VAL(headers):NULL, cc?ZSTR_VAL(cc):NULL,
			bcc?ZSTR_VAL(bcc):NULL, rpath?ZSTR_VAL(rpath):NULL)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}