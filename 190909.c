   Remove the delete flag from a message */
PHP_FUNCTION(imap_undelete)
{
	zval *streamind, *sequence;
	zend_long flags = 0;
	pils *imap_le_struct;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rz|l", &streamind, &sequence, &flags) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	convert_to_string_ex(sequence);

	mail_clearflag_full(imap_le_struct->imap_stream, Z_STRVAL_P(sequence), "\\DELETED", (argc == 3 ? flags : NIL));
	RETVAL_TRUE;