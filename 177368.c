PHP_FUNCTION(mb_ereg_search_setpos)
{
	zend_long position;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &position) == FAILURE) {
		return;
	}

	/* Accept negative position if length of search string can be determined */
	if ((position < 0) && (!Z_ISUNDEF(MBREX(search_str))) && (Z_TYPE(MBREX(search_str)) == IS_STRING)) {
		position += Z_STRLEN(MBREX(search_str));
	}

	if (position < 0 || (!Z_ISUNDEF(MBREX(search_str)) && Z_TYPE(MBREX(search_str)) == IS_STRING && (size_t)position > Z_STRLEN(MBREX(search_str)))) {
		php_error_docref(NULL, E_WARNING, "Position is out of range");
		MBREX(search_pos) = 0;
		RETURN_FALSE;
	}

	MBREX(search_pos) = position;
	RETURN_TRUE;
}