static php_iconv_err_t php_iconv_stream_filter_unregister_factory(TSRMLS_D)
{
	if (FAILURE == php_stream_filter_unregister_factory(
				php_iconv_stream_filter_ops.label TSRMLS_CC)) {
		return PHP_ICONV_ERR_UNKNOWN;
	}
	return PHP_ICONV_ERR_SUCCESS;
}