   Move specified message to a mailbox */
PHP_FUNCTION(imap_mail_move)
{
	zval *streamind;
	zend_string *seq, *folder;
	zend_long options = 0;
	pils *imap_le_struct;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rSS|l", &streamind, &seq, &folder, &options) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_copy_full(imap_le_struct->imap_stream, ZSTR_VAL(seq), ZSTR_VAL(folder), (argc == 4 ? (options | CP_MOVE) : CP_MOVE)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}