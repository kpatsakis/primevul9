PHP_FUNCTION(pclose)
{
	zval *arg1;
	php_stream *stream;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &arg1) == FAILURE) {
		RETURN_FALSE;
	}

	PHP_STREAM_TO_ZVAL(stream, &arg1);

	FG(pclose_wait) = 1;
	zend_list_delete(stream->rsrc_id);
	FG(pclose_wait) = 0;
	RETURN_LONG(FG(pclose_ret));
}