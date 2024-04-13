static PHP_FUNCTION(libxml_set_streams_context)
{
	zval *arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg) == FAILURE) {
		return;
	}
	if (LIBXML(stream_context)) {
		zval_ptr_dtor(&LIBXML(stream_context));
		LIBXML(stream_context) = NULL;
	}
	Z_ADDREF_P(arg);
	LIBXML(stream_context) = arg;
}