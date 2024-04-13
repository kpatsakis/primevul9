   Get a specific body section */
PHP_FUNCTION(imap_fetchbody)
{
	zval *streamind;
	long msgno, flags = 0;
	pils *imap_le_struct;
	char *body, *sec;
	int sec_len;
	unsigned long len;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "rls|l", &streamind, &msgno, &sec, &sec_len, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~(FT_UID|FT_PEEK|FT_INTERNAL)) != 0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(imap_le_struct, pils *, &streamind, -1, "imap", le_imap);

	if (argc < 4 || !(flags & FT_UID)) {
		/* only perform the check if the msgno is a message number and not a UID */
		PHP_IMAP_CHECK_MSGNO(msgno);
	}

	body = mail_fetchbody_full(imap_le_struct->imap_stream, msgno, sec, &len, (argc == 4 ? flags : NIL));

	if (!body) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No body information available");
		RETURN_FALSE;
	}
	RETVAL_STRINGL_CHECK(body, len, 1);