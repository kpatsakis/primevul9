   Return the current PHP version */
PHP_FUNCTION(phpversion)
{
	char *ext_name = NULL;
	int ext_name_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &ext_name, &ext_name_len) == FAILURE) {
		return;
	}

	if (!ext_name) {
		RETURN_STRING(PHP_VERSION, 1);
	} else {
		const char *version;
		version = zend_get_module_version(ext_name);
		if (version == NULL) {
			RETURN_FALSE;
		}
		RETURN_STRING(version, 1);
	}