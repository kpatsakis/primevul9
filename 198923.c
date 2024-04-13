static int _php_mb_regex_globals_ctor(zend_mb_regex_globals *pglobals TSRMLS_DC)
{
	pglobals->default_mbctype = ONIG_ENCODING_EUC_JP;
	pglobals->current_mbctype = ONIG_ENCODING_EUC_JP;
	zend_hash_init(&(pglobals->ht_rc), 0, NULL, (void (*)(void *)) php_mb_regex_free_cache, 1);
	pglobals->search_str = (zval*) NULL;
	pglobals->search_re = (php_mb_regex_t*)NULL;
	pglobals->search_pos = 0;
	pglobals->search_regs = (OnigRegion*)NULL;
	pglobals->regex_default_options = ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE;
	pglobals->regex_default_syntax = ONIG_SYNTAX_RUBY;
	return SUCCESS;
}