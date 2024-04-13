   Decode BASE64 encoded text */
PHP_FUNCTION(imap_base64)
{
	zend_string *text;
	char *decode;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &text) == FAILURE) {
		return;
	}

	decode = (char *) rfc822_base64((unsigned char *) ZSTR_VAL(text), ZSTR_LEN(text), &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength);
	fs_give((void**) &decode);