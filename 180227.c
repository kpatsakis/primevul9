php_stream *php_stream_url_wrap_http(php_stream_wrapper *wrapper, const char *path, const char *mode, int options, zend_string **opened_path, php_stream_context *context STREAMS_DC) /* {{{ */
{
	return php_stream_url_wrap_http_ex(wrapper, path, mode, options, opened_path, context, PHP_URL_REDIRECT_MAX, HTTP_WRAPPER_HEADER_INIT STREAMS_CC);
}