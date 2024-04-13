   Will set the quota for qroot mailbox */
PHP_FUNCTION(imap_set_quota)
{
	zval *streamind;
	zend_string *qroot;
	zend_long mailbox_size;
	pils *imap_le_struct;
	STRINGLIST	limits;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSl", &streamind, &qroot, &mailbox_size) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	limits.text.data = (unsigned char*)"STORAGE";
	limits.text.size = mailbox_size;
	limits.next = NIL;

	RETURN_BOOL(imap_setquota(imap_le_struct->imap_stream, ZSTR_VAL(qroot), &limits));