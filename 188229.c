   Return the actual loaded ini filename */
PHP_FUNCTION(php_ini_loaded_file)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (php_ini_opened_path) {
		RETURN_STRING(php_ini_opened_path, 1);
	} else {
		RETURN_FALSE;
	}