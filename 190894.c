   Permanently delete all messages marked for deletion */
PHP_FUNCTION(imap_expunge)
{
	zval *streamind;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	mail_expunge (imap_le_struct->imap_stream);

	RETURN_TRUE;