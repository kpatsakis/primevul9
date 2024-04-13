PHP_MINFO_FUNCTION(mb_regex)
{
	char buf[32];
	php_info_print_table_start();
	php_info_print_table_row(2, "Multibyte (japanese) regex support", "enabled");
	snprintf(buf, sizeof(buf), "%d.%d.%d",
			ONIGURUMA_VERSION_MAJOR,
			ONIGURUMA_VERSION_MINOR,
			ONIGURUMA_VERSION_TEENY);
#ifdef PHP_ONIG_BUNDLED
#ifdef USE_COMBINATION_EXPLOSION_CHECK
	php_info_print_table_row(2, "Multibyte regex (oniguruma) backtrack check", "On");
#else	/* USE_COMBINATION_EXPLOSION_CHECK */
	php_info_print_table_row(2, "Multibyte regex (oniguruma) backtrack check", "Off");
#endif	/* USE_COMBINATION_EXPLOSION_CHECK */
#endif /* PHP_BUNDLED_ONIG */
	php_info_print_table_row(2, "Multibyte regex (oniguruma) version", buf);
	php_info_print_table_end();
}