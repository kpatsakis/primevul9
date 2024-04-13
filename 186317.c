PHPAPI zend_string *php_session_create_id(PS_CREATE_SID_ARGS) /* {{{ */
{
	unsigned char rbuf[PS_MAX_SID_LENGTH + PS_EXTRA_RAND_BYTES];
	zend_string *outid;

	/* Read additional PS_EXTRA_RAND_BYTES just in case CSPRNG is not safe enough */
	if (php_random_bytes_throw(rbuf, PS(sid_length) + PS_EXTRA_RAND_BYTES) == FAILURE) {
		return NULL;
	}

	outid = zend_string_alloc(PS(sid_length), 0);
	ZSTR_LEN(outid) = bin_to_readable(rbuf, PS(sid_length), ZSTR_VAL(outid), (char)PS(sid_bits_per_character));

	return outid;
}