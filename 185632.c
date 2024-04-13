#ifdef HAVE_IMAP_MUTF7
static void php_imap_mutf7(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
	char *in;
	int in_len;
	unsigned char *out;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &in, &in_len) == FAILURE) {
		return;
	}

	if (in_len < 1) {
		RETURN_EMPTY_STRING();
	}

	if (mode == 0) {
		out = utf8_to_mutf7((unsigned char *) in);
	} else {
		out = utf8_from_mutf7((unsigned char *) in);
	}

	if (out == NIL) {
		RETURN_FALSE;
	} else {
		RETVAL_STRINGL_CHECK((char *)out, strlen(out), 1);
	}