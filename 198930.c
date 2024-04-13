void php_mb_regex_globals_free(zend_mb_regex_globals *pglobals TSRMLS_DC)
{
	if (!pglobals) {
		return;
	}
	_php_mb_regex_globals_dtor(pglobals TSRMLS_CC);
	pefree(pglobals, 1);
}