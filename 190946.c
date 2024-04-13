   Sets flags on messages */
PHP_FUNCTION(imap_setflag_full)
{
	zval *streamind;
	zend_string *sequence, *flag;
	zend_long flags = 0;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS|l", &streamind, &sequence, &flag, &flags) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	mail_setflag_full(imap_le_struct->imap_stream, ZSTR_VAL(sequence), ZSTR_VAL(flag), (flags ? flags : NIL));
	RETURN_TRUE;