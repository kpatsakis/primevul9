static void php_mb_regex_free_cache(zval *el) {
	onig_free((php_mb_regex_t *)Z_PTR_P(el));
}