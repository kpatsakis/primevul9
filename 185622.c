   Read the message body */
PHP_FUNCTION(imap_body)
{
	zval *streamind;
	long msgno, flags = 0;
	pils *imap_le_struct;
	int msgindex, argc = ZEND_NUM_ARGS();
	char *body;
	unsigned long body_len = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "rl|l", &streamind, &msgno, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~(FT_UID|FT_PEEK|FT_INTERNAL)) != 0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, &streamind, -1, "imap", le_imap);

	if ((argc == 3) && (flags & FT_UID)) {
		/* This should be cached; if it causes an extra RTT to the
		   IMAP server, then that's the price we pay for making
		   sure we don't crash. */
		msgindex = mail_msgno(imap_le_struct->imap_stream, msgno);
	} else {
		msgindex = msgno;
	}
	if ((msgindex < 1) || ((unsigned) msgindex > imap_le_struct->imap_stream->nmsgs)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Bad message number");
		RETURN_FALSE;
	}

	body = mail_fetchtext_full (imap_le_struct->imap_stream, msgno, &body_len, (argc == 3 ? flags : NIL));
	if (body_len == 0) {
		RETVAL_EMPTY_STRING();
	} else {
		RETVAL_STRINGL_CHECK(body, body_len, 1);
	}