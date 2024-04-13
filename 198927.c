const char *php_mb_regex_get_mbctype(TSRMLS_D)
{
	return _php_mb_regex_mbctype2name(MBREX(current_mbctype));
}