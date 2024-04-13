   Convert a mime-encoded text to UTF-8 */
PHP_FUNCTION(imap_utf8)
{
	zend_string *str;
	SIZEDTEXT src, dest;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}

	src.data  = NULL;
	src.size  = 0;
	dest.data = NULL;
	dest.size = 0;

	cpytxt(&src, ZSTR_VAL(str), ZSTR_LEN(str));

#ifndef HAVE_NEW_MIME2TEXT
	utf8_mime2text(&src, &dest);
#else
	utf8_mime2text(&src, &dest, U8T_DECOMPOSE);
#endif
	RETVAL_STRINGL((char*)dest.data, dest.size);
	if (dest.data) {
		free(dest.data);
	}
	if (src.data && src.data != dest.data) {
		free(src.data);
	}