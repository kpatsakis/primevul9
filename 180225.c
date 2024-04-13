static int php_stream_http_stream_stat(php_stream_wrapper *wrapper, php_stream *stream, php_stream_statbuf *ssb) /* {{{ */
{
	/* one day, we could fill in the details based on Date: and Content-Length:
	 * headers.  For now, we return with a failure code to prevent the underlying
	 * file's details from being used instead. */
	return -1;
}