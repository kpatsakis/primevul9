PHP_FUNCTION(mb_regex_encoding)
{
	char *encoding = NULL;
	size_t encoding_len;
	OnigEncoding mbctype;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &encoding, &encoding_len) == FAILURE) {
		return;
	}

	if (!encoding) {
		const char *retval = _php_mb_regex_mbctype2name(MBREX(current_mbctype));

		if (retval == NULL) {
			RETURN_FALSE;
		}

		RETURN_STRING((char *)retval);
	} else {
		mbctype = _php_mb_regex_name2mbctype(encoding);

		if (mbctype == ONIG_ENCODING_UNDEF) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", encoding);
			RETURN_FALSE;
		}

		MBREX(current_mbctype) = mbctype;
		RETURN_TRUE;
	}
}