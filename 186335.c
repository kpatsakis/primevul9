static PHP_FUNCTION(session_encode)
{
	zend_string *enc;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	enc = php_session_encode();
	if (enc == NULL) {
		RETURN_FALSE;
	}

	RETURN_STR(enc);
}