int php_mb_regex_set_mbctype(const char *encname)
{
	OnigEncoding mbctype = _php_mb_regex_name2mbctype(encname);
	if (mbctype == ONIG_ENCODING_UNDEF) {
		return FAILURE;
	}
	MBREX(current_mbctype) = mbctype;
	return SUCCESS;
}