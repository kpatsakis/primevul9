static int _php_mb_regex_globals_ctor(zend_mb_regex_globals *pglobals)
{
	pglobals->default_mbctype = ONIG_ENCODING_UTF8;
	pglobals->current_mbctype = ONIG_ENCODING_UTF8;
	ZVAL_UNDEF(&pglobals->search_str);
	pglobals->search_re = (php_mb_regex_t*)NULL;
	pglobals->search_pos = 0;
	pglobals->search_regs = (OnigRegion*)NULL;
	pglobals->regex_default_options = ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE;
	pglobals->regex_default_syntax = ONIG_SYNTAX_RUBY;
	return SUCCESS;
}