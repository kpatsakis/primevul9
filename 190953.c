   Read the full structure of a message */
PHP_FUNCTION(imap_fetchstructure)
{
	zval *streamind;
	zend_long msgno, flags = 0;
	pils *imap_le_struct;
	BODY *body;
	int msgindex, argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rl|l", &streamind, &msgno, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~FT_UID) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (msgno < 1) {
		RETURN_FALSE;
	}

	object_init(return_value);

	if ((argc == 3) && (flags & FT_UID)) {
		/* This should be cached; if it causes an extra RTT to the
		   IMAP server, then that's the price we pay for making
		   sure we don't crash. */
		msgindex = mail_msgno(imap_le_struct->imap_stream, msgno);
	} else {
		msgindex = msgno;
	}
	PHP_IMAP_CHECK_MSGNO(msgindex);

	mail_fetchstructure_full(imap_le_struct->imap_stream, msgno, &body , (argc == 3 ? flags : NIL));

	if (!body) {
		php_error_docref(NULL, E_WARNING, "No body information available");
		RETURN_FALSE;
	}

	_php_imap_add_body(return_value, body);