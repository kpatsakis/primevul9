   Gives the number of messages in the current mailbox */
PHP_FUNCTION(imap_num_msg)
{
	zval *streamind;
	pils *imap_le_struct;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &streamind) == FAILURE) {
		return;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(imap_le_struct->imap_stream->nmsgs);