   Convert an 8-bit string to a quoted-printable string */
PHP_FUNCTION(imap_8bit)
{
	char *text, *decode;
	int text_len;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &text, &text_len) == FAILURE) {
		return;
	}

	decode = (char *) rfc822_8bit((unsigned char *) text, text_len, &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL_CHECK(decode, newlength, 1);
	fs_give((void**) &decode);