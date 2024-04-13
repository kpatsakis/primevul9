	Save a specific body section to a file */
PHP_FUNCTION(imap_savebody)
{
	zval *stream, *out;
	pils *imap_ptr = NULL;
	php_stream *writer = NULL;
	zend_string *section = NULL;
	int close_stream = 1;
	zend_long msgno, flags = 0;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS(), "rzl|Sl", &stream, &out, &msgno, &section, &flags)) {
		RETURN_FALSE;
	}

	if ((imap_ptr = (pils *)zend_fetch_resource(Z_RES_P(stream), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	if (!imap_ptr) {
		RETURN_FALSE;
	}

	switch (Z_TYPE_P(out))
	{
		case IS_LONG:
		case IS_RESOURCE:
			close_stream = 0;
			php_stream_from_zval(writer, out);
		break;

		default:
			convert_to_string_ex(out);
			writer = php_stream_open_wrapper(Z_STRVAL_P(out), "wb", REPORT_ERRORS, NULL);
		break;
	}

	if (!writer) {
		RETURN_FALSE;
	}

	IMAPG(gets_stream) = writer;
	mail_parameters(NIL, SET_GETS, (void *) php_mail_gets);
	mail_fetchbody_full(imap_ptr->imap_stream, msgno, section?ZSTR_VAL(section):"", NULL, flags);
	mail_parameters(NIL, SET_GETS, (void *) NULL);
	IMAPG(gets_stream) = NULL;

	if (close_stream) {
		php_stream_close(writer);
	}

	RETURN_TRUE;