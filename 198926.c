int php_mb_regex_set_default_mbctype(const char *encname TSRMLS_DC)
{
	OnigEncoding mbctype = _php_mb_regex_name2mbctype(encname);
	if (mbctype == ONIG_ENCODING_UNDEF) {
		return FAILURE;
	}
	MBREX(default_mbctype) = mbctype;
	return SUCCESS;
}