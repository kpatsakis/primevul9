	Sets the ACL for a given mailbox */
PHP_FUNCTION(imap_setacl)
{
	zval *streamind;
	zend_string *mailbox, *id, *rights;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSSS", &streamind, &mailbox, &id, &rights) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_BOOL(imap_setacl(imap_le_struct->imap_stream, ZSTR_VAL(mailbox), ZSTR_VAL(id), ZSTR_VAL(rights)));