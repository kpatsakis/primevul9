static php_stream_filter *php_iconv_stream_filter_factory_create(const char *name, zval *params, int persistent TSRMLS_DC)
{
	php_stream_filter *retval = NULL;
	php_iconv_stream_filter *inst;
	char *from_charset = NULL, *to_charset = NULL;
	size_t from_charset_len, to_charset_len;

	if ((from_charset = strchr(name, '.')) == NULL) {
		return NULL;
	}
	++from_charset;
	if ((from_charset = strchr(from_charset, '.')) == NULL) {
		return NULL;
	}
	++from_charset;
	if ((to_charset = strpbrk(from_charset, "/.")) == NULL) {
		return NULL;
	}
	from_charset_len = to_charset - from_charset;
	++to_charset;
	to_charset_len = strlen(to_charset);

	if (from_charset_len >= ICONV_CSNMAXLEN || to_charset_len >= ICONV_CSNMAXLEN) {
		return NULL;
	}

	if (NULL == (inst = pemalloc(sizeof(php_iconv_stream_filter), persistent))) {
		return NULL;
	}

	if (php_iconv_stream_filter_ctor(inst, to_charset, to_charset_len, from_charset, from_charset_len, persistent) != PHP_ICONV_ERR_SUCCESS) {
		pefree(inst, persistent);
		return NULL;
	}

	if (NULL == (retval = php_stream_filter_alloc(&php_iconv_stream_filter_ops, inst, persistent))) {
		php_iconv_stream_filter_dtor(inst);
		pefree(inst, persistent);
	}

	return retval;
}