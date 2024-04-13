PHP_FUNCTION(mb_ereg_search_setpos)
{
	long position;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &position) == FAILURE) {
		return;
	}

	if (position < 0 || (MBREX(search_str) != NULL && Z_TYPE_P(MBREX(search_str)) == IS_STRING && position >= Z_STRLEN_P(MBREX(search_str)))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Position is out of range");
		MBREX(search_pos) = 0;
		RETURN_FALSE;
	}

	MBREX(search_pos) = position;
	RETURN_TRUE;
}