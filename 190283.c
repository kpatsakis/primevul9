static php_iconv_err_t php_iconv_stream_filter_register_factory(TSRMLS_D)
{
	static php_stream_filter_factory filter_factory = {
		php_iconv_stream_filter_factory_create
	};

	if (FAILURE == php_stream_filter_register_factory(
				php_iconv_stream_filter_ops.label,
				&filter_factory TSRMLS_CC)) {
		return PHP_ICONV_ERR_UNKNOWN;
	}
	return PHP_ICONV_ERR_SUCCESS;
}