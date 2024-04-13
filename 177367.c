PHP_FUNCTION(mb_eregi_replace)
{
	_php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, ONIG_OPTION_IGNORECASE, 0);
}