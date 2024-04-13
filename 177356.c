zend_mb_regex_globals *php_mb_regex_globals_alloc(void)
{
	zend_mb_regex_globals *pglobals = pemalloc(
			sizeof(zend_mb_regex_globals), 1);
	if (SUCCESS != _php_mb_regex_globals_ctor(pglobals)) {
		pefree(pglobals, 1);
		return NULL;
	}
	return pglobals;
}