   Convert an 8bit string to a base64 string */
PHP_FUNCTION(imap_binary)
{
	char *text, *decode;
	int text_len;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &text, &text_len) == FAILURE) {
		return;
	}

	decode = rfc822_binary(text, text_len, &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL_CHECK(decode, newlength, 1);
	fs_give((void**) &decode);