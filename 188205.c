   Return comma-separated string of .ini files parsed from the additional ini dir */
PHP_FUNCTION(php_ini_scanned_files)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (strlen(PHP_CONFIG_FILE_SCAN_DIR) && php_ini_scanned_files) {
		RETURN_STRING(php_ini_scanned_files, 1);
	} else {
		RETURN_FALSE;
	}