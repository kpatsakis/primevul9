	Gets the ACL for a given mailbox */
PHP_FUNCTION(imap_getacl)
{
	zval *streamind;
	zend_string *mailbox;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS", &streamind, &mailbox) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	/* initializing the special array for the return values */
	array_init(return_value);

	IMAPG(imap_acl_list) = return_value;

	/* set the callback for the GET_ACL function */
	mail_parameters(NIL, SET_ACL, (void *) mail_getacl);
	if (!imap_getacl(imap_le_struct->imap_stream, ZSTR_VAL(mailbox))) {
		php_error(E_WARNING, "c-client imap_getacl failed");
		zval_dtor(return_value);
		RETURN_FALSE;
	}

	IMAPG(imap_acl_list) = NIL;