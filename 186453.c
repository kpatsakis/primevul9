PHP_LIBXML_API void php_libxml_structured_error_handler(void *userData, xmlErrorPtr error)
{
	_php_list_set_error_structure(error, NULL);

	return;
}