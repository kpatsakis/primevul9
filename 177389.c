void php_mb_regex_globals_free(zend_mb_regex_globals *pglobals)
{
	if (!pglobals) {
		return;
	}
	_php_mb_regex_globals_dtor(pglobals);
	pefree(pglobals, 1);
}