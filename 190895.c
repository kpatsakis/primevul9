   Convert a quoted-printable string to an 8-bit string */
PHP_FUNCTION(imap_qprint)
{
	zend_string *text;
	char *decode;
	unsigned long newlength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &text) == FAILURE) {
		return;
	}

	decode = (char *) rfc822_qprint((unsigned char *) ZSTR_VAL(text), ZSTR_LEN(text), &newlength);

	if (decode == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(decode, newlength);
	fs_give((void**) &decode);