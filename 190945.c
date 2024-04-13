   Get the unique message id associated with a standard sequential message number */
PHP_FUNCTION(imap_uid)
{
	zval *streamind;
	zend_long msgno;
	pils *imap_le_struct;
	int msgindex;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &streamind, &msgno) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	msgindex = msgno;
	if ((msgindex < 1) || ((unsigned) msgindex > imap_le_struct->imap_stream->nmsgs)) {
		php_error_docref(NULL, E_WARNING, "Bad message number");
		RETURN_FALSE;
	}

	RETURN_LONG(mail_uid(imap_le_struct->imap_stream, msgno));