   Get the sequence number associated with a UID */
PHP_FUNCTION(imap_msgno)
{
	zval *streamind;
	zend_long msgno;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &streamind, &msgno) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(mail_msgno(imap_le_struct->imap_stream, msgno));