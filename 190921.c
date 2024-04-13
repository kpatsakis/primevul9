   This function garbage collects (purges) the cache of entries of a specific type. */
PHP_FUNCTION(imap_gc)
{
	zval *streamind;
	pils *imap_le_struct;
	zend_long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rl", &streamind, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~(GC_TEXTS | GC_ELT | GC_ENV)) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the flags parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	mail_gc(imap_le_struct->imap_stream, flags);

	RETURN_TRUE;