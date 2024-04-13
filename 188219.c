   Return the current SAPI module name */
PHP_FUNCTION(php_sapi_name)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (sapi_module.name) {
		RETURN_STRING(sapi_module.name, 1);
	} else {
		RETURN_FALSE;
	}