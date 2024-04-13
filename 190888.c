   Read the message body */
PHP_FUNCTION(imap_body)
{
	zval *streamind;
	zend_long msgno, flags = 0;
	pils *imap_le_struct;
	int msgindex, argc = ZEND_NUM_ARGS();
	char *body;
	unsigned long body_len = 0;

	if (zend_parse_parameters(argc, "rl|l", &streamind, &msgno, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~(FT_UID|FT_PEEK|FT_INTERNAL)) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if ((argc == 3) && (flags & FT_UID)) {
		/* This should be cached; if it causes an extra RTT to the
		   IMAP server, then that's the price we pay for making
		   sure we don't crash. */
		msgindex = mail_msgno(imap_le_struct->imap_stream, msgno);
	} else {
		msgindex = msgno;
	}
	if ((msgindex < 1) || ((unsigned) msgindex > imap_le_struct->imap_stream->nmsgs)) {
		php_error_docref(NULL, E_WARNING, "Bad message number");
		RETURN_FALSE;
	}

	body = mail_fetchtext_full (imap_le_struct->imap_stream, msgno, &body_len, (argc == 3 ? flags : NIL));
	if (body_len == 0) {
		RETVAL_EMPTY_STRING();
	} else {
		RETVAL_STRINGL(body, body_len);
	}