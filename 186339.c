static PHP_FUNCTION(session_get_cookie_params)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);

	add_assoc_long(return_value, "lifetime", PS(cookie_lifetime));
	add_assoc_string(return_value, "path", PS(cookie_path));
	add_assoc_string(return_value, "domain", PS(cookie_domain));
	add_assoc_bool(return_value, "secure", PS(cookie_secure));
	add_assoc_bool(return_value, "httponly", PS(cookie_httponly));
}