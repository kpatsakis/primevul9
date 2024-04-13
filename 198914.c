zend_mb_regex_globals *php_mb_regex_globals_alloc(TSRMLS_D)
{
	zend_mb_regex_globals *pglobals = pemalloc(
			sizeof(zend_mb_regex_globals), 1);
	if (!pglobals) {
		return NULL;
	}
	if (SUCCESS != _php_mb_regex_globals_ctor(pglobals TSRMLS_CC)) {
		pefree(pglobals, 1);
		return NULL;
	}
	return pglobals;
}