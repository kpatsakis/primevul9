   Append a new message to a specified mailbox */
PHP_FUNCTION(imap_append)
{
	zval *streamind;
	zend_string *folder, *message, *internal_date = NULL, *flags = NULL;
	pils *imap_le_struct;
	STRING st;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rSS|SS", &streamind, &folder, &message, &flags, &internal_date) == FAILURE) {
		return;
	}


	if (internal_date) {
		zend_string *regex  = zend_string_init("/[0-3][0-9]-((Jan)|(Feb)|(Mar)|(Apr)|(May)|(Jun)|(Jul)|(Aug)|(Sep)|(Oct)|(Nov)|(Dec))-[0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] [+-][0-9]{4}/", sizeof("/[0-3][0-9]-((Jan)|(Feb)|(Mar)|(Apr)|(May)|(Jun)|(Jul)|(Aug)|(Sep)|(Oct)|(Nov)|(Dec))-[0-9]{4} [0-2][0-9]:[0-5][0-9]:[0-5][0-9] [+-][0-9]{4}/") - 1, 0);
		pcre_cache_entry *pce;				/* Compiled regex */
		zval *subpats = NULL;				/* Parts (not used) */
		int global = 0;

		/* Make sure the given internal_date string matches the RFC specifiedformat */
		if ((pce = pcre_get_compiled_regex_cache(regex))== NULL) {
			zend_string_free(regex);
			RETURN_FALSE;
		}

		zend_string_free(regex);
		php_pcre_match_impl(pce, ZSTR_VAL(internal_date), ZSTR_LEN(internal_date), return_value, subpats, global,
			0, Z_L(0), Z_L(0));

		if (!Z_LVAL_P(return_value)) {
			php_error_docref(NULL, E_WARNING, "internal date not correctly formatted");
			internal_date = NULL;
		}
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	INIT (&st, mail_string, (void *) ZSTR_VAL(message), ZSTR_LEN(message));

	if (mail_append_full(imap_le_struct->imap_stream, ZSTR_VAL(folder), (flags ? ZSTR_VAL(flags) : NIL), (internal_date ? ZSTR_VAL(internal_date) : NIL), &st)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}