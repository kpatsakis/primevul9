static php_mb_regex_t *php_mbregex_compile_pattern(const char *pattern, size_t patlen, OnigOptionType options, OnigEncoding enc, OnigSyntaxType *syntax)
{
	int err_code = 0;
	php_mb_regex_t *retval = NULL, *rc = NULL;
	OnigErrorInfo err_info;
	OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];

	rc = zend_hash_str_find_ptr(&MBREX(ht_rc), (char *)pattern, patlen);
	if (!rc || onig_get_options(rc) != options || onig_get_encoding(rc) != enc || onig_get_syntax(rc) != syntax) {
		if ((err_code = onig_new(&retval, (OnigUChar *)pattern, (OnigUChar *)(pattern + patlen), options, enc, syntax, &err_info)) != ONIG_NORMAL) {
			onig_error_code_to_str(err_str, err_code, &err_info);
			php_error_docref(NULL, E_WARNING, "mbregex compile err: %s", err_str);
			retval = NULL;
			goto out;
		}
		if (rc == MBREX(search_re)) {
			/* reuse the new rc? see bug #72399 */
			MBREX(search_re) = NULL;
		}
		zend_hash_str_update_ptr(&MBREX(ht_rc), (char *)pattern, patlen, retval);
	} else {
		retval = rc;
	}
out:
	return retval;
}