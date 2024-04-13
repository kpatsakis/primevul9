#ifdef HAVE_IMAP_MUTF7
static void php_imap_mutf7(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
	zend_string *in;
	unsigned char *out;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &in) == FAILURE) {
		return;
	}

	if (ZSTR_LEN(in) < 1) {
		RETURN_EMPTY_STRING();
	}

	if (mode == 0) {
		out = utf8_to_mutf7((unsigned char *) ZSTR_VAL(in));
	} else {
		out = utf8_from_mutf7((unsigned char *) ZSTR_VAL(in));
	}

	if (out == NIL) {
		RETURN_FALSE;
	} else {
		RETURN_STRING((char *)out);
	}