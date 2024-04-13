PHP_LIBXML_API zval *php_libxml_switch_context(zval *context TSRMLS_DC)
{
	zval *oldcontext;

	oldcontext = LIBXML(stream_context);
	LIBXML(stream_context) = context;
	return oldcontext;

}