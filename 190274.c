static int php_iconv_output_handler(void **nothing, php_output_context *output_context)
{
	char *s, *content_type, *mimetype = NULL;
	int output_status, mimetype_len = 0;
	PHP_OUTPUT_TSRMLS(output_context);

	if (output_context->op & PHP_OUTPUT_HANDLER_START) {
		output_status = php_output_get_status(TSRMLS_C);
		if (output_status & PHP_OUTPUT_SENT) {
			return FAILURE;
		}

		if (SG(sapi_headers).mimetype && !strncasecmp(SG(sapi_headers).mimetype, "text/", 5)) {
			if ((s = strchr(SG(sapi_headers).mimetype,';')) == NULL){
				mimetype = SG(sapi_headers).mimetype;
			} else {
				mimetype = SG(sapi_headers).mimetype;
				mimetype_len = s - SG(sapi_headers).mimetype;
			}
		} else if (SG(sapi_headers).send_default_content_type) {
			mimetype = SG(default_mimetype) ? SG(default_mimetype) : SAPI_DEFAULT_MIMETYPE;
		}

		if (mimetype != NULL && !(output_context->op & PHP_OUTPUT_HANDLER_CLEAN)) {
			int len;
			char *p = strstr(get_output_encoding(TSRMLS_C), "//");

			if (p) {
				len = spprintf(&content_type, 0, "Content-Type:%.*s; charset=%.*s", mimetype_len ? mimetype_len : (int) strlen(mimetype), mimetype, (int)(p - get_output_encoding(TSRMLS_C)), get_output_encoding(TSRMLS_C));
			} else {
				len = spprintf(&content_type, 0, "Content-Type:%.*s; charset=%s", mimetype_len ? mimetype_len : (int) strlen(mimetype), mimetype, get_output_encoding(TSRMLS_C));
			}
			if (content_type && SUCCESS == sapi_add_header(content_type, len, 0)) {
				SG(sapi_headers).send_default_content_type = 0;
				php_output_handler_hook(PHP_OUTPUT_HANDLER_HOOK_IMMUTABLE, NULL TSRMLS_CC);
			}
		}
	}

	if (output_context->in.used) {
		output_context->out.free = 1;
		_php_iconv_show_error(php_iconv_string(output_context->in.data, output_context->in.used, &output_context->out.data, &output_context->out.used, get_output_encoding(TSRMLS_C), get_internal_encoding(TSRMLS_C)), get_output_encoding(TSRMLS_C), get_internal_encoding(TSRMLS_C) TSRMLS_CC);
	}

	return SUCCESS;
}