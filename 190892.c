   Check if the IMAP stream is still active */
PHP_FUNCTION(imap_ping)
{
	zval *streamind;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_BOOL(mail_ping(imap_le_struct->imap_stream));