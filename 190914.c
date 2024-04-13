   Rename a mailbox */
PHP_FUNCTION(imap_renamemailbox)
{
	zval *streamind;
	zend_string *old_mailbox, *new_mailbox;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS", &streamind, &old_mailbox, &new_mailbox) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (mail_rename(imap_le_struct->imap_stream, ZSTR_VAL(old_mailbox), ZSTR_VAL(new_mailbox)) == T) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}