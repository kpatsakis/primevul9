   Create a new mailbox */
PHP_FUNCTION(imap_createmailbox)
{
	zval *streamind;
	zend_string *folder;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS", &streamind, &folder) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_create(imap_le_struct->imap_stream, ZSTR_VAL(folder)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}