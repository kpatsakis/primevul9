file_replace(struct magic_set *ms, const char *pat, const char *rep)
{
	zval *patt;
	int opts = 0;
	pcre_cache_entry *pce;
	char *res;
	zval *repl;
	int res_len, rep_cnt = 0;
	TSRMLS_FETCH();

	MAKE_STD_ZVAL(patt);
	ZVAL_STRINGL(patt, pat, strlen(pat), 0);
	opts |= PCRE_MULTILINE;
	convert_libmagic_pattern(patt, opts);
	if ((pce = pcre_get_compiled_regex_cache(Z_STRVAL_P(patt), Z_STRLEN_P(patt) TSRMLS_CC)) == NULL) {
		zval_dtor(patt);
		FREE_ZVAL(patt);
		return -1;
	}

	MAKE_STD_ZVAL(repl);
	ZVAL_STRINGL(repl, rep, strlen(rep), 0);

	res = php_pcre_replace_impl(pce, ms->o.buf, strlen(ms->o.buf), repl,
			0, &res_len, -1, &rep_cnt TSRMLS_CC);

	FREE_ZVAL(repl);
	zval_dtor(patt);
	FREE_ZVAL(patt);

	if (NULL == res) {
		return -1;
	}

	strncpy(ms->o.buf, res, res_len);
	ms->o.buf[res_len] = '\0';

	efree(res);

	return rep_cnt;
}