	Returns the quota set to the mailbox account mbox */
PHP_FUNCTION(imap_get_quotaroot)
{
	zval *streamind;
	zend_string *mbox;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rS", &streamind, &mbox) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);
	IMAPG(quota_return) = &return_value;

	/* set the callback for the GET_QUOTAROOT function */
	mail_parameters(NIL, SET_QUOTA, (void *) mail_getquota);
	if (!imap_getquotaroot(imap_le_struct->imap_stream, ZSTR_VAL(mbox))) {
		php_error_docref(NULL, E_WARNING, "c-client imap_getquotaroot failed");
		zval_dtor(return_value);
		RETURN_FALSE;
	}