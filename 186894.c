file_magwarn(struct magic_set *ms, const char *f, ...)
{
	va_list va;
	char *expanded_format;
	TSRMLS_FETCH();

	va_start(va, f);
	vasprintf(&expanded_format, f, va);
	va_end(va);
	
	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Warning: %s", expanded_format);

	free(expanded_format);
}