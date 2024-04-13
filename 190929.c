   Get a specific body section */
PHP_FUNCTION(imap_fetchbody)
{
	zval *streamind;
	zend_long msgno, flags = 0;
	pils *imap_le_struct;
	char *body;
	zend_string *sec;
	unsigned long len;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rlS|l", &streamind, &msgno, &sec, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~(FT_UID|FT_PEEK|FT_INTERNAL)) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (argc < 4 || !(flags & FT_UID)) {
		/* only perform the check if the msgno is a message number and not a UID */
		PHP_IMAP_CHECK_MSGNO(msgno);
	}

	body = mail_fetchbody_full(imap_le_struct->imap_stream, msgno, ZSTR_VAL(sec), &len, (argc == 4 ? flags : NIL));

	if (!body) {
		php_error_docref(NULL, E_WARNING, "No body information available");
		RETURN_FALSE;
	}
	RETVAL_STRINGL(body, len);