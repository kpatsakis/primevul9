static void php_iconv_stream_filter_cleanup(php_stream_filter *filter TSRMLS_DC)
{
	php_iconv_stream_filter_dtor((php_iconv_stream_filter *)filter->abstract);
	pefree(filter->abstract, ((php_iconv_stream_filter *)filter->abstract)->persistent);
}