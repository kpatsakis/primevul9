PHP_RINIT_FUNCTION(mb_regex)
{
	if (!MBSTRG(mb_regex_globals)) return FAILURE;
	zend_hash_init(&MBREX(ht_rc), 0, NULL, php_mb_regex_free_cache, 0);
	return SUCCESS;
}