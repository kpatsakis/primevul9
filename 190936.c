   Copy specified message to a mailbox */
PHP_FUNCTION(imap_mail_copy)
{
	zval *streamind;
	zend_long options = 0;
	zend_string *seq, *folder;
	int argc = ZEND_NUM_ARGS();
	pils *imap_le_struct;

	if (zend_parse_parameters(argc, "rSS|l", &streamind, &seq, &folder, &options) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_copy_full(imap_le_struct->imap_stream, ZSTR_VAL(seq), ZSTR_VAL(folder), (argc == 4 ? options : NIL)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}