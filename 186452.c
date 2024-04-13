static void php_libxml_internal_error_handler(int error_type, void *ctx, const char **msg, va_list ap)
{
	char *buf;
	int len, len_iter, output = 0;

	TSRMLS_FETCH();

	len = vspprintf(&buf, 0, *msg, ap);
	len_iter = len;

	/* remove any trailing \n */
	while (len_iter && buf[--len_iter] == '\n') {
		buf[len_iter] = '\0';
		output = 1;
	}

	smart_str_appendl(&LIBXML(error_buffer), buf, len);

	efree(buf);

	if (output == 1) {
		if (LIBXML(error_list)) {
			_php_list_set_error_structure(NULL, LIBXML(error_buffer).c);
		} else {
			switch (error_type) {
				case PHP_LIBXML_CTX_ERROR:
					php_libxml_ctx_error_level(E_WARNING, ctx, LIBXML(error_buffer).c TSRMLS_CC);
					break;
				case PHP_LIBXML_CTX_WARNING:
					php_libxml_ctx_error_level(E_NOTICE, ctx, LIBXML(error_buffer).c TSRMLS_CC);
					break;
				default:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", LIBXML(error_buffer).c);
			}
		}
		smart_str_free(&LIBXML(error_buffer));
	}
}