const char *php_mb_regex_get_default_mbctype(TSRMLS_D)
{
	return _php_mb_regex_mbctype2name(MBREX(default_mbctype));
}