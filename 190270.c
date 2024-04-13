PHP_MSHUTDOWN_FUNCTION(miconv)
{
	php_iconv_stream_filter_unregister_factory(TSRMLS_C);
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}