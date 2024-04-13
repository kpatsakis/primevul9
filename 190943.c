   Get the full unfiltered header for a message */
PHP_FUNCTION(imap_fetchheader)
{
	zval *streamind;
	zend_long msgno, flags = 0L;
	pils *imap_le_struct;
	int msgindex, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rl|l", &streamind, &msgno, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~(FT_UID|FT_INTERNAL|FT_PREFETCHTEXT)) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if ((argc == 3) && (flags & FT_UID)) {
		/* This should be cached; if it causes an extra RTT to the
		   IMAP server, then that's the price we pay for making sure
		   we don't crash. */
		msgindex = mail_msgno(imap_le_struct->imap_stream, msgno);
	} else {
		msgindex = msgno;
	}

	PHP_IMAP_CHECK_MSGNO(msgindex);

	RETVAL_STRING(mail_fetchheader_full(imap_le_struct->imap_stream, msgno, NIL, NIL, (argc == 3 ? flags : NIL)));