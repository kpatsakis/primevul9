static php_output_handler *php_iconv_output_handler_init(const char *handler_name, size_t handler_name_len, size_t chunk_size, int flags TSRMLS_DC)
{
	return php_output_handler_create_internal(handler_name, handler_name_len, php_iconv_output_handler, chunk_size, flags TSRMLS_CC);
}